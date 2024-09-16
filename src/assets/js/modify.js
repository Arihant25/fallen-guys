// TODO: Update the API key and fields

let submitBtn = document.getElementById("submit");

submitBtn.addEventListener("click", function (event) {
  event.preventDefault();
  var accelerometer = Number(document.getElementById("accelerometer").value);
  var gyroscope = Number(document.getElementById("gyroscope").value);
  var gps = Number(document.getElementById("gps").value);
  var battery = Number(document.getElementById("battery").value);
  var password = document.getElementById("password").value;
  var accepted = document.getElementById("accepted").checked;

  if (isNaN(accelerometer) || accelerometer === 0)
    alert("Invalid accelerometer threshold (must be a positive number)");
  else if (isNaN(gyroscope) || gyroscope === 0)
    alert("Invalid gyroscope threshold (must be a positive number)");
  else if (isNaN(gps) || gps === 0)
    alert("Invalid heat index threshold");
  else if (isNaN(battery) || battery === 0)
    alert("Invalid battery threshold (must be a positive number)");
  else if (!accepted) alert("Please accept the terms and conditions");
  else {
    fetch(
      `https://api.thingspeak.com/update?api_key=DMUU4N34U556W1W1&field1=${accelerometer}&field2=${gps}&field3=${gyroscope}&field4=${battery}&field5=${password}`
    )
      .then((response) => response.json())
      .then((data) => {
        if (data === 0) alert("Error sending data");
        else {
          alert("Data sent successfully");
          window.location.href = "index.html";
        }
      })
      .catch((error) => {
        alert("Error sending data", error);
      });
  }
});
