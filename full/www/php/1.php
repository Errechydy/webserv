<?php

    session_start();

    $_SESSION["name"] = "Er-rechydy";

    $cookie_name = "user";
    $cookie_value = "John Doe";
    setcookie("user", "John Doe", time() + (86400 * 30), "/"); // 86400 = 1 day


    echo "Session and cookies has been set";


?>