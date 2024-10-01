function FallenGuys() {
  $(function () {
    // Display GPS Location on Map
    var map = L.map('map').setView([17, 78], 11);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      maxZoom: 19,
      attribution: '© OpenStreetMap contributors'
    }).addTo(map);
    var marker = L.marker([17, 78]).addTo(map);
    
    // Add click event listener to the map
    map.on('click', function(e) {
      var lat = e.latlng.lat;
      var lng = e.latlng.lng;
      var googleMapsUrl = `https://www.google.com/maps?q=${lat},${lng}`;
      window.open(googleMapsUrl, '_blank');
    });

    fetch(
      "https://api.thingspeak.com/channels/2658268/feeds.json?api_key=FRIN88PUMDP9QGS4"
    )
      .then((response) => response.json())
      .then((init_data) => {
        fetch(
          `https://api.thingspeak.com/channels/2658268/feeds.json?api_key=FRIN88PUMDP9QGS4&results=${init_data.channel.last_entry_id}`
        )
          .then((response) => response.json())
          .then((data) => {
            var fetched_data = data.feeds;

            // =====================================
            // Angular Velocity
            // =====================================
            var NangVel = {
              chart: {
                id: "sparkline6",
                type: "area",
                height: 60,
                sparkline: {
                  enabled: true,
                },
                group: "sparklines",
                fontFamily: "Plus Jakarta Sans', sans-serif",
                foreColor: "#adb0bb",
              },
              series: [
                {
                  name: "Net Angular Velocity",
                  color: "#5073b3",
                  data: fetched_data
                    .slice(-100)
                    .map((item) =>
                      Math.sqrt(
                        parseFloat(item.field4) ** 2 +
                        parseFloat(item.field5) ** 2 +
                        parseFloat(item.field6) ** 2
                      ).toFixed(1)
                    ),
                },
              ],
              stroke: {
                curve: "smooth",
                width: 2,
              },
              fill: {
                colors: ["#f3feff"],
                type: "solid",
                opacity: 0.05,
              },
              markers: {
                size: 0,
              },
              tooltip: {
                theme: "dark",
                fixed: {
                  enabled: true,
                  position: "right",
                },
                x: {
                  show: false,
                },
              },
            };
            new ApexCharts(
              document.querySelector("#NangVelPlot"),
              NangVel
            ).render();
            document.querySelector("#NangVel").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field6).toFixed(
                1
              ) + " rad/s";

            // =====================================
            // Max NEt Acceleration
            // =====================================
            var maxNetAcc = {
              chart: {
                id: "sparkline7",
                type: "area",
                height: 60,
                sparkline: {
                  enabled: true,
                },
                group: "sparklines",
                fontFamily: "Plus Jakarta Sans', sans-serif",
                foreColor: "#adb0bb",
              },
              series: [
                {
                  name: "Max Net Acceleration",
                  color: "#29e7cd",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field8).toFixed(1)),
                },
              ],
              stroke: {
                curve: "smooth",
                width: 2,
              },
              fill: {
                colors: ["#f3feff"],
                type: "solid",
                opacity: 0.05,
              },
              markers: {
                size: 0,
              },
              tooltip: {
                theme: "dark",
                fixed: {
                  enabled: true,
                  position: "right",
                },
                x: {
                  show: false,
                },
              },
            };
            new ApexCharts(
              document.querySelector("#maxnetaccPlot"),
              maxNetAcc
            ).render();
            document.querySelector("#maxnetacc").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field8).toFixed(
                1
              ) + " m/s²";

            // =====================================
            // GPS Coordinates
            // =====================================
            var lastEntry = fetched_data[fetched_data.length - 1];
            var lat = parseFloat(lastEntry.field7.split(" ")[0]);
            var lon = parseFloat(lastEntry.field7.split(" ")[1]);

            if (!isNaN(lat) && !isNaN(lon)) {
              document.querySelector("#gps").textContent = lat.toFixed(6) + ', ' + lon.toFixed(6);
              marker.setLatLng([lat, lon]);
              map.setView([lat, lon], 13);
            } else {
              document.querySelector("#gps").textContent = "No GPS data available";
            }

            // =====================================
            // Past Alerts
            // =====================================
            var alertList = document.getElementById("alertList");
            alertList.innerHTML = `<li
                      class="timeline-item d-flex position-relative overflow-hidden"
                      id="lastAlert"
                    >
                      <div
                        class="timeline-time text-dark flex-shrink-0 text-end"
                        id="startup"
                      >
                        <div>2024-04-09</div>
                        <div style="padding-bottom: 5vh">22:08:05 UTC</div>
                      </div>
                      <div
                        class="timeline-badge-wrap d-flex flex-column align-items-center"
                      >
                        <span
                          class="timeline-badge border-2 border border-success flex-shrink-0 my-8"
                        ></span>
                      </div>
                      <div class="timeline-desc fs-3 text-dark mt-n1">
                        Node started
                      </div>
                    </li>`;

            document.getElementById("startup").innerHTML = `<div>${fetched_data[0].created_at.split("T")[0]
              }</div>
        <div style="padding-bottom: 5vh;">${fetched_data[0].created_at.split("T")[1].replace("Z", "") + " UTC"
              }</div>`;

            var lastAlert = document.getElementById("lastAlert");
            for (var i = fetched_data.length - 1; i >= 0; i--) {
              if (parseFloat(fetched_data[i].field8) >= 15.0) {
                var li = document.createElement("li");
                li.className =
                  "timeline-item d-flex position-relative overflow-hidden";

                const utcDate = new Date(fetched_data[i].created_at);
                const istDate = new Date(
                  utcDate.toLocaleString("en-US", { timeZone: "Asia/Kolkata" })
                );
                var div1 = document.createElement("div");
                div1.className =
                  "timeline-time text-dark flex-shrink-0 text-end";
                var date = document.createElement("div");
                date.textContent = istDate
                  .toLocaleDateString("en-US", {
                    day: "2-digit",
                    month: "2-digit",
                    year: "numeric",
                  })
                  .replace("/", "-")
                  .replace("/", "-");
                div1.appendChild(date);
                var time = document.createElement("div");
                time.textContent =
                  istDate.toLocaleTimeString("en-US", { hour12: false }) +
                  " IST";
                div1.appendChild(time);

                var div2 = document.createElement("div");
                div2.className =
                  "timeline-badge-wrap d-flex flex-column align-items-center";

                var span1 = document.createElement("span");
                span1.className =
                  "timeline-badge border-2 border border-danger flex-shrink-0 my-8";

                var span2 = document.createElement("span");
                span2.className = "timeline-badge-border d-block flex-shrink-0";

                div2.appendChild(span1);
                div2.appendChild(span2);

                var div3 = document.createElement("div");
                div3.className =
                  "timeline-desc fs-3 text-dark mt-n1 fw-semibold";
                div3.textContent = "Fall Detected! ";

                var fallAcc = document.createElement("span");
                fallAcc.className = "text-primary d-block fw-normal";
                fallAcc.textContent = `Fall Acceleration: ${parseInt(
                  fetched_data[i].field8
                )} m/s^2`;
                div3.appendChild(fallAcc);
                var fallAngVel = document.createElement("span");
                fallAngVel.className = "text-primary d-block fw-normal";
                fallAngVel.textContent = `Angular Velocity: ${Math.sqrt(
                  parseFloat(fetched_data[i].field4) ** 2 +
                  parseFloat(fetched_data[i].field5) ** 2 +
                  parseFloat(fetched_data[i].field6) ** 2
                ).toFixed(1)} rad/s`;
                div3.appendChild(fallAngVel);
                var GPSLoc = document.createElement("span");
                GPSLoc.className = "text-primary d-block fw-normal";
                GPSLoc.textContent = `GPS Location: None`;
                div3.appendChild(GPSLoc);

                li.appendChild(div1);
                li.appendChild(div2);
                li.appendChild(div3);

                lastAlert.parentNode.insertBefore(li, lastAlert);
              }
            }

            // =====================================
            // Current Alert
            // =====================================
            if (fetched_data[fetched_data.length - 1]["field8"] === "1") {
              document.getElementById("fallAlert").style.backgroundColor =
                "rgb(168, 34, 50)";
              document.getElementById("fallAlert").style.borderRadius = "13px";

              document.getElementById("alertText").innerHTML = "Fall Detected!";
              document.getElementById("alertText").style.color = "white";

              document.getElementById("alertLogo").src =
                "../assets/images/logos/favicon.png";
              document.getElementById("alertLogo").alt = "Fall Alert";
            }
            else {
              document.getElementById("fallAlert").style.backgroundColor =
                "white";
              document.getElementById("fallAlert").style.borderRadius = "13px";

              document.getElementById("alertText").innerHTML = "All Good!";
              document.getElementById("fallAlert").style.color = "black";

              document.getElementById("alertLogo").src =
                "../assets/images/checkmark.png";
              document.getElementById("alertLogo").alt = "Checkmark";
            }
          })
          .catch((error) => {
            console.error(error);
          });
      });
  });
}

FallenGuys();
setInterval(FallenGuys, 15000);
