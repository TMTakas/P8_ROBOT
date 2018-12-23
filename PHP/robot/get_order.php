<?php

    $servername = "localhost";
    $username = "xxxx";
    $password = "xxxx";
    $dbname = "xxxx";

try
{
    $lastcommande = $_GET["lastcommande"];
    $temperature = $_GET["temperature"];
    $humidity = $_GET["humidity"];
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    // set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    // Set Last Commande As Executed
    $conn->exec("UPDATE robot set executed = 1 WHERE id = '". $lastcommande ."'");
    
    $conn->exec("INSERT INTO dht(temperature,humidity ) VALUES(".$temperature.",".$humidity.")");
    
    $req = "SELECT * FROM robot WHERE executed = 0 ORDER BY id ASC LIMIT 1";
    $stmt = $conn->prepare($req);
    $stmt->execute();
    $data = $stmt->fetchAll();
    $found = false;
    foreach ($data as $row)
    {
        echo "|" . $row["id"] . "|" .  $row["order_to_exe"] . "|";
        $found = true;
    }
    if(!$found)
    {
        echo "NoCmd";
    }
}
catch(PDOException $e)
{
    echo $e->getMessage() . " ERRORSQL";
}

$conn = null;
?>