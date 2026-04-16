let lock = false;

async function prt(e) {
  e.preventDefault();
  if (lock) return;

  try {
    lock = true;
    showloading(true);
    closeWarnings();  // clear any previous diagnostics

    const x = document.getElementById("container").value;
    const res = await fetch("http://localhost:8000/run", {
      method: "POST",
      headers: { 'Content-Type': 'text/plain' },
      body: x
    });

    const text = await res.text();
    const result = JSON.parse(text);

    // Show C output
    document.getElementById("output").value = result.output || '';

    // Show compiler warnings / errors if any
    if (result.warnings && result.warnings.length > 0) {
      showWarnings(result.warnings);
    }

    // Show invalid-input alert if output is missing
    if (!result.output || result.output.trim() === "") {
      showalert();
    }

  } catch (err) {
    console.log("ERROR:", err);
  } finally {
    lock = false;
    showloading(false);
  }
}

/* ---- Warnings panel ---- */
function showWarnings(text) {
  const panel  = document.getElementById("warnings-panel");
  const body   = document.getElementById("warnings-body");
  const icon   = document.getElementById("warnings-icon");

  body.textContent = text;
  panel.style.display = "block";

  // Use red styling if any line starts with "Error", amber for "Warning"
  const hasError = text.split('\n').some(l => l.trim().startsWith("Error"));
  if (hasError) {
    panel.classList.add("has-errors");
    icon.textContent = "✖";
  } else {
    panel.classList.remove("has-errors");
    icon.textContent = "⚠";
  }
}

function closeWarnings() {
  const panel = document.getElementById("warnings-panel");
  panel.style.display = "none";
  panel.classList.remove("has-errors");
  document.getElementById("warnings-body").textContent = "";
}

/* ---- Alert overlay ---- */
function showalert() {
  document.getElementById("alert").style.display = "flex";
  document.getElementById("alert_outer").style.display = "flex";
  document.getElementsByTagName("body")[0].style.opacity = "0.5";
}

function closealert() {
  document.getElementById("alert").style.display = "none";
  document.getElementById("alert_outer").style.display = "none";
  document.getElementsByTagName("body")[0].style.opacity = "1";
}

/* ---- Loading state ---- */
function showloading(val) {
  const btn = document.getElementById("load");
  if (val) {
    btn.textContent = "Compiling...";
    btn.style.opacity = "0.5";
    btn.style.pointerEvents = "none";
  } else {
    btn.textContent = "Run";
    btn.style.opacity = "1";
    btn.style.pointerEvents = "auto";
  }
}

/* ---- Reset ---- */
function reset() {
  document.getElementById("container").value = "";
  document.getElementById("output").value = "";
  closeWarnings();
}