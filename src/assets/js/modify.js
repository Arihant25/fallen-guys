let submitBtn = document.getElementById("submit");

submitBtn.addEventListener("click", function (event) {
  event.preventDefault();
  var fallAcc = document.getElementById("fallAcc").value;
  var alert = document.getElementById("alert").value;
  var emergencyContact = document.getElementById("emergencyContact").value;
  var password = document.getElementById("password").value;
  var accepted = document.getElementById("accepted").checked;

  if (isNaN(fallAcc))
    alert("Invalid Fall Acceleration Threshold!");
  else if (isNaN(alert))
    alert("Invalid Alert Delay!");
  else if (isNaN(emergencyContact) || heatIndex === 0)
    alert("Invalid Emergency Contact!");
  else if (!accepted)
    alert("Please accept the terms and conditions");
  else {
    // TODO: GET request to push this data to thingspeak server
  //   fetch(
  //     `https://api.thingspeak.com/update?api_key=DMUU4N34U556W1W1&field1=${temperature}&field2=${heatIndex}&field3=${humidity}&field4=${smoke}&field5=${password}&field6=${flame}`
  //   )
  //     .then((response) => response.json())
  //     .then((data) => {
  //       if (data === 0) alert("Error sending data");
  //       else {
  //         alert("Data sent successfully");
  //         window.location.href = "index.html";
  //       }
  //     })
  //     .catch((error) => {
  //       alert("Error sending data", error);
  //     });
  // }
});