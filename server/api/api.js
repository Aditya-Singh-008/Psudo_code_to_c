import express from 'express';
import { writeFile, readFile } from 'fs/promises';
import { execFile } from 'child_process';   // execFile: no Node-side shell spawned
import path from 'path';
import { fileURLToPath } from 'url';

// Resolve paths relative to THIS file so they work regardless of where npm start is called from
const __dirname = path.dirname(fileURLToPath(import.meta.url));
const LOGIC_DIR = path.resolve(__dirname, '../../logic');

const INPUT_FILE  = path.join(LOGIC_DIR, '.test.txt');
const OUTPUT_FILE = path.join(LOGIC_DIR, '.output.txt');

const router = express.Router();

router.post("/", async (req, res) => {
    try {
        let code = req.body;
        let lang = 'c';
        try {
            const parsed = JSON.parse(req.body);
            if (parsed.code !== undefined) {
                code = parsed.code;
                lang = (parsed.lang && parsed.lang.trim() !== '') ? parsed.lang.toLowerCase() : 'c';
            }
        } catch (e) {
            // Fallback: it was sent as pure text instead of JSON
        }

        // 1. Overwrite the shared input file with the latest pseudocode
        await writeFile(INPUT_FILE, code);

        // 2. Convert Windows absolute path → WSL /mnt/... path
        const toWsl = (p) =>
            p.replace(/\\/g, '/').replace(/^([A-Z]):/i, (_, d) => `/mnt/${d.toLowerCase()}`);

        const wslInput  = toWsl(INPUT_FILE);
        const wslOutput = toWsl(OUTPUT_FILE);
        const wslDir    = toWsl(LOGIC_DIR);

        // 3. Run compiler via WSL (execFile — no Node-side shell)
        execFile(
            'wsl',
            ['bash', '-c', `cd '${wslDir}' && ./my_compiler '${lang}' < '${wslInput}' > '${wslOutput}'`],
            async (error, _stdout, stderr) => {
                if (error) {
                    console.error("Compile Error:", stderr);
                    return res.status(500).send({ error: stderr || error.message });
                }

                try {
                    // 3. Read compiler output into memory
                    const output = await readFile(OUTPUT_FILE, 'utf8');

                    // 4. Capture any compiler warnings/errors from stderr
                    //    (e.g. undeclared variables, duplicate declarations)
                    const warnings = stderr ? stderr.trim() : '';

                    res.status(201).send({ msg: "Executed Successfully", output, warnings });
                } catch (readErr) {
                    console.error("Read Error:", readErr);
                    res.status(500).send({ error: "Failed to read compiler output." });
                }
            }
        );

    } catch (err) {
        console.error("Server Error:", err);
        res.status(500).send({ error: "Internal Server Error" });
    }
});

export default router;
