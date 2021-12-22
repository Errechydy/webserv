<?php

session_start();

$_SESSION["name"] = "EL";

//---------
$cookie_name = "user";
$cookie_value = "John Doe";
setcookie("user", "John Doe", time() + (86400 * 30), "/"); // 86400 = 1 day


// if(!isset($_COOKIE["user"])) {
//     echo "Cookie named 'user' is not set!";
// } else {
//     echo "Cookie 'user' is set!<br>";
//     echo "Value is: " . $_COOKIE['user'];
// }




echo "my value = ".$_GET['userId'].", v = ".$_POST['v'];


?>