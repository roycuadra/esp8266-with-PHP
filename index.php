<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "weather_db";

try {
    // Establish database connection using PDO
    $conn = new PDO("mysql:host=$servername;dbname=$dbname;charset=utf8", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Handle incoming sensor data
    if ($_SERVER["REQUEST_METHOD"] === "POST") {
        $temperature = $_POST['temperature'] ?? null;
        $humidity = $_POST['humidity'] ?? null;

        if ($temperature !== null && $humidity !== null) {
            $stmt = $conn->prepare("INSERT INTO sensor_data (temperature, humidity) VALUES (:temperature, :humidity)");
            $stmt->bindParam(':temperature', $temperature);
            $stmt->bindParam(':humidity', $humidity);
            $stmt->execute();
            echo json_encode(["status" => "success", "message" => "Data stored successfully"]);
        } elseif (isset($_POST['delete_all'])) {
            // Handle delete all request
            try {
                $stmt = $conn->prepare("DELETE FROM sensor_data");
                $stmt->execute();
                echo json_encode(["status" => "success", "message" => "All records deleted"]);
            } catch (PDOException $e) {
                echo json_encode(["status" => "error", "message" => "Error deleting records: " . $e->getMessage()]);
            }
            exit; // Prevent further execution
        } else {
            echo json_encode(["status" => "error", "message" => "Missing temperature or humidity data"]);
        }
        exit; // Prevent further execution
    }

    // Fetch sensor data
    $stmt = $conn->query("SELECT * FROM sensor_data ORDER BY timestamp DESC");
    $sensorData = $stmt->fetchAll(PDO::FETCH_ASSOC);

} catch (PDOException $e) {
    die("Database error: " . $e->getMessage());
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Weather Sensor Data</title>
    <link rel="stylesheet" href="styles.css">
    <script src="./assets/jquery.js"></script>
</head>
<body>
    <div class="container">
        <h2>Weather Sensor Data</h2>
        <div class="table-responsive">
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Temperature (°C)</th>
                        <th>Humidity (%)</th>
                        <th>Timestamp</th>
                    </tr>
                </thead>
                <tbody id="sensorTable">
                    <!-- Data will be populated here by AJAX -->
                    <?php foreach ($sensorData as $row): ?>
                        <tr>
                            <td><?php echo $row['id']; ?></td>
                            <td><?php echo $row['temperature']; ?></td>
                            <td><?php echo $row['humidity']; ?></td>
                            <td><?php echo $row['timestamp']; ?></td>
                        </tr>
                    <?php endforeach; ?>
                </tbody>
            </table>
        </div>
        <div class="footer">
            <button onclick="exportToExcel()" class="btn">Export to Excel</button>
            <button onclick="deleteAllData()" class="btn">Delete All</button>
        </div>
    </div>
      <script src="script.js"></script>
</body>
</html>
