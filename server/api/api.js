import express from 'express';
import { writeFile } from 'fs/promises';
import { exec } from 'child_process'; // Use exec for async-friendly flow
import path from 'path';
import { readFileSync } from 'fs';

const router = express.Router();

router.post("/", async (req, res) => {
    try {
        const data = req.body;
        // 1. Save the file (await ensures it's finished before we compile)
        await writeFile("../logic/.test.txt", data);

        // 2. Map Windows paths to WSL paths
        const winPath = path.resolve("../logic/.test.txt");
        const wslPath = winPath.replace(/\\/g, '/').replace(/^([A-Z]):/i, (_, drive) => `/mnt/${drive.toLowerCase()}`);

        const outputPath = wslPath.replace('.test.txt', '.output.txt');
        const compileDir = path.dirname(wslPath);
        const command = `wsl bash -c "cd ${compileDir} && ./my_compiler < ${wslPath} > ${outputPath}"`;

        exec(command, (error, stdout, stderr) => {
            if (error) {
                console.error("WSL Error:", stderr);
                return res.status(500).send({ error: stderr || error.message });
            }
            // If the command succeeds, stdout might be empty because we redirected to output.txt
            const data= readFileSync("../logic/.output.txt")
            res.status(201).send({msg:"Executed Successful",output:`${data}`})
        });

    } catch (err) {
        console.error("Server Error:", err);
        res.status(500).send({ err: "Internal Server Error" });
    }
});

export default router;
