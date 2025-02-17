<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "weather_db";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname;charset=utf8", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $stmt = $conn->query("SELECT * FROM sensor_data ORDER BY timestamp DESC");
    $sensorData = $stmt->fetchAll(PDO::FETCH_ASSOC);
    
    if (!empty($sensorData)) {
        foreach ($sensorData as $row) {
            echo "<tr>
                    <td>" . htmlspecialchars($row['id']) . "</td>
                    <td>" . htmlspecialchars($row['temperature']) . " °C</td>
                    <td>" . htmlspecialchars($row['humidity']) . " %</td>
                    <td>" . htmlspecialchars($row['timestamp']) . "</td>
                  </tr>";
        }
    } else {
        echo "<tr><td colspan='4' class='text-center'>No data available</td></tr>";
    }
} catch (PDOException $e) {
    die("Database error: " . $e->getMessage());
}
?>
