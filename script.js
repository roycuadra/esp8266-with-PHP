function fetchData() {
  $.ajax({
    url: "fetch_data.php",
    method: "GET",
    success: function (response) {
      $("#sensorTable").html(response);
      updateTimestamps(); // Ensure timestamps are updated after data is loaded
    },
    error: function (xhr, status, error) {
      console.error("Error fetching data:", error);
    }
  });
}

function exportToExcel() {
  let table = document.getElementById("sensorTable");
  let rows = table.getElementsByTagName("tr");
  let csv = [];

  let header = [];
  let headers = table.getElementsByTagName("th"); 
  for (let i = 0; i < headers.length; i++) {
    header.push(headers[i].innerText.trim());
  }
  csv.push(header.join(","));

  // Loop through rows
  for (let i = 0; i < rows.length; i++) {
    let row = [];
    let cells = rows[i].getElementsByTagName("td");
    for (let j = 0; j < cells.length; j++) {
      let cellData = cells[j].innerText.trim();
      if (j === 3) { // Assuming timestamp is in column index 3
        cellData = `"${cellData}"`; // Keep timestamp format
      }
      row.push(cellData);
    }
    csv.push(row.join(","));
  }

  let csvContent = "data:text/csv;charset=utf-8," + csv.join("\n");
  let encodedUri = encodeURI(csvContent);
  let link = document.createElement("a");
  link.setAttribute("href", encodedUri);
  link.setAttribute("download", "sensor_data.csv");
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
}


function deleteAllData() {
  if (confirm("Are you sure you want to delete all records?")) {
    $.ajax({
      url: "index.php",
      method: "POST",
      data: { delete_all: true },
      success: function (response) {
        try {
          let result = JSON.parse(response);
          if (result.status === "success") {
            alert(result.message);
            fetchData();
          } else {
            alert(result.message);
          }
        } catch (e) {
          console.error("Invalid JSON response:", response);
        }
      },
      error: function (xhr, status, error) {
        console.error("Error deleting data:", error);
      }
    });
  }
}

$(document).ready(function () {
  fetchData();
  setInterval(fetchData, 3000);
});

function formatTimestampTo12Hour(timestamp) {
  if (!timestamp) return "Invalid Time"; // Handle empty or invalid timestamps

  let dateParts = timestamp.split(/[- :]/); // Handle "YYYY-MM-DD HH:MM:SS"
  if (dateParts.length < 5) return "Invalid Time"; // Ensure correct format

  let date = new Date(
    dateParts[0], // Year
    dateParts[1] - 1, // Month (zero-based)
    dateParts[2], // Day
    dateParts[3], // Hour
    dateParts[4], // Minute
    dateParts[5] // Second
  );

  let hours = date.getHours();
  let minutes = date.getMinutes();
  let seconds = date.getSeconds();
  let ampm = hours >= 12 ? "PM" : "AM";

  // Convert to 12-hour format
  hours = hours % 12 || 12;
  minutes = minutes < 10 ? "0" + minutes : minutes;
  seconds = seconds < 10 ? "0" + seconds : seconds;

  return `${hours}:${minutes}:${seconds} ${ampm}`;
}

function updateTimestamps() {
  let rows = document.querySelectorAll("#sensorTable tr");
  rows.forEach(function (row) {
    let timestampCell = row.cells[3]; // Assuming timestamp is in the 4th column
    if (timestampCell) {
      let originalTimestamp = timestampCell.innerText.trim();
      timestampCell.innerText = formatTimestampTo12Hour(originalTimestamp);
    }
  });
}

// Ensure timestamps update after fetching data
$(document).ready(function () {
  fetchData();
  setInterval(fetchData, 3000);
});
