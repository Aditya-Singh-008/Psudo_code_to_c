let lock=false
async function prt(e){
  e.preventDefault();
  if(lock)return
    
    try{
      lock=true;
      showloading(true);
      let x = document.getElementById("container").value;
      const res = await fetch("http://localhost:8000/run", {
        method: "POST",
        headers: { 'Content-Type': 'text/plain' },
        body: x
      });
      
      console.log("Response received", res);
      
      const text = await res.text();   // 👈 NOT json yet
      console.log("RAW RESPONSE:", text);
      
      const result = JSON.parse(text); // 👈 force parse
      const value=result.output;
      if(value=="undefined"){
        showalert();
      }
      document.getElementById("output").value = result.output;
      
    } catch(err){
      console.log("ERROR:", err);
    }finally{
      lock=false
      showloading(false)
    }
}
function showalert(){
  let x = document.getElementById("alert");
  let y = document.getElementById("alert_outer");
  x.style.display="flex";
  y.style.display="flex";
  document.getElementsByTagName("body").style.opacity=0.5;
  document.getElementById("*").style.filter=brightness(0.5);
}
function closealert(){
  let x = document.getElementById("alert");
  let y = document.getElementById("alert_outer");
  x.style.display="none";
  y.style.display="none";
  document.getElementsByTagName("body").style.opacity=1;
  document.getElementById("*").style.filter="blur(0)brightness(1)";
}
function showloading(val){
  let btn=document.getElementById("load")
  if(val)
  {
    btn.textContent = "Compiling..."; // Added quotes
    btn.style.opacity = "0.5";         // Visual logic: dim the button
    btn.style.pointerEvents = "none";
  }
  else{
    btn.textContent="Run";
    btn.style.opacity = "1";
    btn.style.pointerEvents = "auto";
  }
}
function reset() {
  document.getElementById("container").value = "";
  document.getElementById("output").value = "";
}