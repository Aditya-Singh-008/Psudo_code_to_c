import { appendFile } from 'fs'
async function logging(req,res,next){
    try
    {

        const data=`\n${new Date().toLocaleTimeString()}     ${req.url}`
        await appendFile("./logs/.logs.txt",data,(err)=>{
            if(err)console.log(err)
        })
        }catch(err){
            console.log("eroor is",err)
    }
    next()
}
export default logging