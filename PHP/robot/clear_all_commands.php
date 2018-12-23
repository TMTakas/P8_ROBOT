<?php


    $servername = "localhost";
    $username = "xxxx";
    $password = "xxxx";
    $dbname = "xxxx";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    // set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "UPDATE robot SET executed = 1";
    // use exec() because no results are returned
    $conn->exec($sql);
    echo "All Commands Are Cleared";
    }
catch(PDOException $e)
    {
    echo $sql . "<br>" . $e->getMessage();
    }

$conn = null;
?>