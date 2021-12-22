<?php

session_start();
echo $_SESSION["name"]."------";


if(!isset($_COOKIE["user"])) {
    echo "Cookie named 'user' is not set!";
} else {
    echo "Cookie 'user' is set!<br>";
    echo "Value is: " . $_COOKIE['user'];
}

?>