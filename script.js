async function prt(e){
  e.preventDefault();
  console.log("RUN clicked");

  try{
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

    document.getElementById("output").value = result.output;

  } catch(err){
    console.log("ERROR:", err);
  }
}

function reset() {
  document.getElementById("container").value = "";
  document.getElementById("output").value = "";
}