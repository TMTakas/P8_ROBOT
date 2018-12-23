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
    
    $req = "SELECT COUNT(*) AS count FROM robot WHERE executed = 0";
    $stmt = $conn->prepare($req);
    $stmt->execute();
    $data = $stmt->fetchAll();
    foreach ($data as $row)
    {
        echo $row["count"];
    }
}
catch(PDOException $e)
{
    echo $e->getMessage() . " ERRORSQL";
}

$conn = null;
?>