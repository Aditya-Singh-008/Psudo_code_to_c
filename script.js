/* ---- Tab Switching ---- */
function switchTab(tab) {
  document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  document.getElementById('tab-' + tab).classList.add('active');
  document.getElementById('panel-' + tab).classList.add('active');
}

/* ---- Sample Codes ---- */
const SAMPLES = [
  {
    title: 'Hello World',
    code:
`START
  SHOW "Hello, World!"
STOP`
  },
  {
    title: 'Simple Calculator',
    code:
`START
  VAR a AS INT
  VAR b AS INT
  VAR result AS INT

  SHOW "Enter first number:"
  GET a
  SHOW "Enter second number:"
  GET b

  SET result TO a + b
  SHOW "Sum:"
  SHOW result

  SET result TO a * b
  SHOW "Product:"
  SHOW result
STOP`
  },
  {
    title: 'Even or Odd',
    code:
`START
  VAR n AS INT
  VAR rem AS INT

  SHOW "Enter a number:"
  GET n

  SET rem TO n - n / 2 * 2
  IF rem == 0
    SHOW "The number is Even"
  ELSE
    SHOW "The number is Odd"
  ENDIF
STOP`
  },
  {
    title: 'Countdown Loop',
    code:
`START
  VAR count AS INT
  SET count TO 10

  SHOW "Counting down:"
  WHILE count > 0
    SHOW count
    SET count TO count - 1
  DONE

  SHOW "Blastoff!"
STOP`
  },
  {
    title: 'Array Sum',
    code:
`START
  VAR scores AS INT ARRAY SIZE 5 WITH VALUES 10 20 30 40 50
  VAR total AS INT
  VAR i AS INT

  SET total TO 0
  SET i TO 0

  WHILE i < 5
    SET total TO total + scores[i]
    SET i TO i + 1
  DONE

  SHOW "Sum of array:"
  SHOW total
STOP`
  },
  {
    title: 'Bubble Sort',
    code:
`START
  VAR arr AS INT ARRAY SIZE 5 WITH VALUES 64 34 25 12 22
  VAR i AS INT
  VAR j AS INT
  VAR temp AS INT
  VAR n AS INT
  SET n TO 5
  
  SET i TO 0
  WHILE i < n - 1
    SET j TO 0
    WHILE j < n - i - 1
      IF arr[j] > arr[j + 1]
        SET temp TO arr[j]
        SET arr[j] TO arr[j + 1]
        SET arr[j + 1] TO temp
      ENDIF
      SET j TO j + 1
    DONE
    SET i TO i + 1
  DONE

  SHOW "Sorted Array:"
  SET i TO 0
  WHILE i < n
    SHOW arr[i]
    SET i TO i + 1
  DONE
STOP`
  },
  {
    title: 'Fibonacci Sequence',
    code:
`START
  VAR n AS INT
  VAR t1 AS INT
  VAR t2 AS INT
  VAR nextTerm AS INT
  VAR i AS INT

  SHOW "Enter number of terms:"
  GET n
  
  SET t1 TO 0
  SET t2 TO 1
  SET i TO 1

  SHOW "Fibonacci Sequence:"
  WHILE i <= n
    SHOW t1
    SET nextTerm TO t1 + t2
    SET t1 TO t2
    SET t2 TO nextTerm
    SET i TO i + 1
  DONE
STOP`
  }
];

function loadSample(index) {
  document.getElementById('container').value = SAMPLES[index].code;
  document.getElementById('output').value = '';
  closeWarnings();

  /* Highlight the active card */
  document.querySelectorAll('.sample-card').forEach((c, i) => {
    c.classList.toggle('active-sample', i === index);
  });
}

let lock = false;


async function prt(e) {
  e.preventDefault();
  if (lock) return;

  try {
    lock = true;
    showloading(true);
    closeWarnings();  // clear any previous diagnostics

    const x = document.getElementById("container").value;
    const lang = document.getElementById("value").value;
    if (x === "" || lang === "") {
      showalert();
      return;
    }
    const res = await fetch("http://localhost:8000/run", {
      method: "POST",
      headers: { 'Content-Type': 'text/plain' },
      body: JSON.stringify({ code: x, lang: lang })
    });

    const text = await res.text();
    const result = JSON.parse(text);

    // Show translated output
    document.getElementById("output").value = result.output || '';

    if (!res.ok) {
      showWarnings(`Error: ${result.error || 'Translation failed.'}`);
      return;
    }

    // Show compiler warnings if any
    if (result.warnings && result.warnings.length > 0) {
      showWarnings(result.warnings);
    }

    // Show invalid-input alert if output is missing
    if (!result.output || result.output.trim() === "") {
      showalert();
    }

  } catch (err) {
    console.log("ERROR:", err);
    showWarnings("Error: Unable to reach the translation server.");
  } finally {
    lock = false;
    showloading(false);
  }
}

/* ---- Warnings panel ---- */
function showWarnings(text) {
  const panel = document.getElementById("warnings-panel");
  const body = document.getElementById("warnings-body");
  const icon = document.getElementById("warnings-icon");

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
