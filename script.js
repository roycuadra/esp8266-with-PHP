function fetchData() {
  $.ajax({
    url: "fetch_data.php",
    method: "GET",
    success: function (response) {
      $("#sensorTable").html(response);
    },
  });
}

function exportToExcel() {
  let table = document.getElementById("sensorTable");
  let rows = table.getElementsByTagName("tr");
  let csv = [];

  let header = [];
  let headers = document.getElementsByTagName("th");
  for (let i = 0; i < headers.length; i++) {
    header.push(headers[i].innerText);
  }
  csv.push(header.join(","));

  // Loop through rows to gather data, including the timestamp
  for (let i = 0; i < rows.length; i++) {
    let row = [];
    let cells = rows[i].getElementsByTagName("td");
    for (let j = 0; j < cells.length; j++) {
      row.push(cells[j].innerText);
    }
    // Ensure timestamp is added to each row (it's already part of the table)
    csv.push(row.join(","));
  }

  // Create CSV content
  let csvContent = "data:text/csv;charset=utf-8," + csv.join("\n");
  let encodedUri = encodeURI(csvContent);
  let link = document.createElement("a");
  link.setAttribute("href", encodedUri);
  link.setAttribute("download", "sensor_data.csv");
  link.click();
}

function deleteAllData() {
  if (confirm("Are you sure you want to delete all records?")) {
    $.ajax({
      url: "index.php", // The same PHP file
      method: "POST",
      data: { delete_all: true },
      success: function (response) {
        let result = JSON.parse(response);
        if (result.status === "success") {
          alert(result.message);
          fetchData(); // Refresh the table
        } else {
          alert(result.message);
        }
      },
    });
  }
}

$(document).ready(function () {
  fetchData();
  setInterval(fetchData, 3000);
});

function formatTimestampTo12Hour(timestamp) {
  let date = new Date(timestamp);
  let hours = date.getHours();
  let minutes = date.getMinutes();
  let seconds = date.getSeconds();
  let ampm = hours >= 12 ? "PM" : "AM";

  // Convert to 12-hour format
  hours = hours % 12;
  hours = hours ? hours : 12; // Handle midnight as 12, not 0
  minutes = minutes < 10 ? "0" + minutes : minutes;
  seconds = seconds < 10 ? "0" + seconds : seconds;

  // Return formatted time
  return hours + ":" + minutes + ":" + seconds + " " + ampm;
}

function updateTimestamps() {
  // Iterate through each row and update the timestamp
  let rows = document.querySelectorAll("#sensorTable tr");
  rows.forEach(function (row) {
    let timestampCell = row.cells[3]; // Assuming the timestamp is in the 4th column
    if (timestampCell) {
      let originalTimestamp = timestampCell.innerText.trim();
      // Convert the timestamp and update the cell content
      timestampCell.innerText = formatTimestampTo12Hour(originalTimestamp);
    }
  });
}

// Call the updateTimestamps function when the page loads or when the table is refreshed
$(document).ready(function () {
  updateTimestamps();
  setInterval(function () {
    fetchData();
    updateTimestamps();
  }, 3000);
});
