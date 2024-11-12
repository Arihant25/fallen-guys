let submitBtn = document.getElementById("submit");

submitBtn.addEventListener("click", function (event) {
  event.preventDefault();
  var fallAcc = document.getElementById("fallAcc").value;
  var alertDelay = document.getElementById("alert").value;
  var emergencyContact = document.getElementById("emergencyContact").value;
  var password = document.getElementById("password").value;
  var mlEnable = document.getElementById("mlEnable").checked;
  var accepted = document.getElementById("accepted").checked;

  if (isNaN(fallAcc)) {
    alert("Invalid Fall Acceleration Threshold!");
  }
  else if (isNaN(alertDelay)) {
    alert("Invalid Alert Delay!");
  }
  else if (isNaN(emergencyContact)) {
    alert("Invalid Emergency Contact!");
  }
  else if (!accepted) {
    alert("Please accept the terms and conditions");
  }
  else {
    fetch(
      `https://api.thingspeak.com/update?api_key=GJRF5PDPH9MKTT54&field1=${fallAcc}&field2=${alertDelay}&field3=${emergencyContact}&field4=${password}&field5=${mlEnable}`,
    )
      .then((response) => response.json())
      .then((data) => {
        if (data === 0) {
            alert("Error sending data");
        }

        else {
          window.location.href = "index.html";
          alert("Data sent successfully");
        }
      })

      .catch((error) => {
        alert("Error sending data", error);
      });
  }
});