import { appendFile, appendFileSync } from 'fs'
function logging(req,res,next){
    try
    {
        const data=`\n${new Date().toLocaleTimeString()}     ${req.url}`
        appendFileSync("./logs/.logs.txt",data,(err)=>{
            if(err)console.log(err)
        })
        }catch(err){
            console.log("eroor is",err)
    }
    next()
}
export default logging