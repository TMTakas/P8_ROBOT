<?php

    $servername = "localhost";
    $username = "xxxx";
    $password = "xxxx";
    $dbname = "xxxx";

try
{
   
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    // set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    

    
    $req = "SELECT CONCAT(HOUR(sent),\"-\",MINUTE(sent)) AS 'HM', AVG(temperature) AS 'temperature',AVG(humidity) AS 'humidity' FROM dht
WHERE YEAR(sent) = YEAR(NOW()) AND MONTH(sent) = MONTH(NOW()) AND DAY(sent) = DAY(NOW()) AND temperature > 0 AND humidity > 0
GROUP BY CONCAT(HOUR(sent),\"-\",MINUTE(sent))
ORDER BY sent ASC";
    $stmt = $conn->prepare($req);
    $stmt->execute();
    $data = $stmt->fetchAll();
    foreach ($data as $row)
    {
        echo $row["HM"] . "|" . $row["temperature"] . "|" . $row["humidity"] . ";";
    }
    //echo json_encode($data);
}
catch(PDOException $e)
{
    echo $e->getMessage() . " ERRORSQL";
}

$conn = null;
?>