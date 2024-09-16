function FallenGuys() {
  $(function () {
    fetch(
      "https://api.thingspeak.com/channels/2658268/feeds.json?api_key=FRIN88PUMDP9QGS4&results=2"
    )
      .then((response) => response.json())
      .then((init_data) => {
        fetch(
          `https://api.thingspeak.com/channels/2658268/feeds.json?api_key=FRIN88PUMDP9QGS4&results=${init_data.channel.last_entry_id}`
        )
          .then((response) => response.json())
          .then((data) => {
            var fetched_data = data.feeds;
            console.log(fetched_data);  

            // =====================================
            // Acceleration
            // =====================================
            var Xacc = {
              chart: {
                id: "sparkline1",
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
                  name: "Xacc",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field1).toFixed(1)),
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
            new ApexCharts(document.querySelector("#Xacc"), Xacc).render();
            document.querySelector("#Xacc").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field1).toFixed(1) + " m/s^2";
            
            var Yacc = {
              chart: {
                id: "sparkline2",
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
                  name: "Yacc",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field2).toFixed(1)),
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
            new ApexCharts(document.querySelector("#Yacc"), Yacc).render();
            document.querySelector("#Yacc").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field2).toFixed(1) + " m/s^2";
            
            var Zacc = {
              chart: {
                id: "sparkline3",
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
                  name: "Zacc",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field3).toFixed(1)),
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
            new ApexCharts(document.querySelector("#Zacc"), Zacc).render();
            document.querySelector("#Zacc").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field3).toFixed(1) + " m/s^2";
            
              var Nacc = {
                chart: {
                  id: "sparkline2",
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
                    name: "Nacc",
                    color: "#FFA500",
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
              new ApexCharts(document.querySelector("#Nacc"), Nacc).render();
              document.querySelector("#Nacc").textContent =
                parseFloat(fetched_data[fetched_data.length - 1].field8).toFixed(1) + " m/s^2";  
            
            var XangVel = {
              chart: {
                id: "sparkline4",
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
                  name: "XangVel",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field4).toFixed(1)),
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
            new ApexCharts(document.querySelector("#XangVel"), XangVel).render();
            document.querySelector("#XangVel").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field4).toFixed(1) + " rad/s";
            
            var YangVel = {
              chart: {
                id: "sparkline5",
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
                  name: "YangVel",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field5).toFixed(1)),
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
            new ApexCharts(document.querySelector("#YangVel"), YangVel).render();
            document.querySelector("#YangVel").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field5).toFixed(1) + " rad/s";
            
            var ZangVel = {
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
                  name: "ZangVel",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field6).toFixed(1)),
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
            new ApexCharts(document.querySelector("#ZangVel"), ZangVel).render();
            document.querySelector("#ZangVel").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field6).toFixed(1) + " rad/s";
            
            var temp = {
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
                  name: "Temperature",
                  color: "#FFA500",
                  data: fetched_data
                    .slice(-100)
                    .map((item) => parseFloat(item.field7).toFixed(1)),
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
            new ApexCharts(document.querySelector("#temp"), temp).render();
            document.querySelector("#temp").textContent =
              parseFloat(fetched_data[fetched_data.length - 1].field7).toFixed(1) + " °C";
            
            // =====================================
            // Alerts
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

            document.getElementById("startup").innerHTML = `<div>${
              fetched_data[0].created_at.split("T")[0]
            }</div>
        <div style="padding-bottom: 5vh;">${
          fetched_data[0].created_at.split("T")[1].replace("Z", "") + " UTC"
        }</div>`;

            var lastAlert = document.getElementById("lastAlert");
            for (var i = fetched_data.length - 1; i >= 0; i--) {
              if (fetched_data[i].field6 === "1") {
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
                div3.textContent = "Fire Detected! ";

                var temp_details = document.createElement("span");
                temp_details.className = "text-primary d-block fw-normal";
                temp_details.textContent = `Temperature: ${parseFloat(
                  fetched_data[i].field1
                ).toFixed(1)} °C`;
                div3.appendChild(temp_details);
                var humidity_details = document.createElement("span");
                humidity_details.className = "text-primary d-block fw-normal";
                humidity_details.textContent = `Humidity: ${parseInt(
                  fetched_data[i].field3
                )} %`;
                div3.appendChild(humidity_details);
                var heatI_details = document.createElement("span");
                heatI_details.className = "text-primary d-block fw-normal";
                heatI_details.textContent = `Heat Index: ${parseFloat(
                  fetched_data[i].field2
                ).toFixed(1)} °C`;
                div3.appendChild(heatI_details);
                var smoke_details = document.createElement("span");
                smoke_details.className = "text-primary d-block fw-normal";
                smoke_details.textContent = `Smoke: ${parseInt(
                  fetched_data[i].field5
                )} PPM`;
                div3.appendChild(smoke_details);
                var flame_details = document.createElement("span");
                flame_details.className = "text-primary d-block fw-normal";
                flame_details.textContent = `Flame: ${parseFloat(
                  100 * (4095 - fetched_data[i].field7) / 4095
                ).toFixed(1)} %`;
                div3.appendChild(flame_details);

                li.appendChild(div1);
                li.appendChild(div2);
                li.appendChild(div3);

                lastAlert.parentNode.insertBefore(li, lastAlert);
              }
            }

            // Check for fire, to raise alert on the webPage
            if (fetched_data[fetched_data.length - 1]["field6"] === "1") {
              document.getElementById("fireAlert").style.backgroundColor =
                "rgb(168, 34, 50)";
              document.getElementById("fireAlert").style.borderRadius = "13px";

              document.getElementById("alertText").innerHTML = "Fall Detected!";
              document.getElementById("alertText").style.color = "white";

              document.getElementById("alertLogo").src =
                "../assets/images/logos/favicon.png";
              document.getElementById("alertLogo").alt = "Fire Alert";
            } else {
              document.getElementById("fireAlert").style.backgroundColor =
                "white";
              document.getElementById("fireAlert").style.borderRadius = "13px";

              document.getElementById("alertText").innerHTML = "All Good!";
              document.getElementById("fireAlert").style.color = "black";

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
setInterval(FallenGuys, 30000);