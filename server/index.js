import express from 'express'
import router from './api/api.js'
import cors from 'cors'
import logging from './api/log.js'
const server= express();
server.use(cors())
server.set('view engine','ejs')
server.set('views','./static');
server.use(express.json())
server.use(express.text())
server.use("/",logging)
server.use(express.urlencoded({extended:false}))
server.get("/",(req,res)=>{
    res.render("error");
})
server.use("/run",router)
server.listen(8000,()=>{
    console.log(`server running @ http://localhost:8000`)
})