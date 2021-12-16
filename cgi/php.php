<?php

    // echo $_GET['a'];
    // echo "hello from php";



    // $DEBUG = true;
// if ($DEBUG) echo "<p>PHP: good to go</p>";
// if ($DEBUG) 
    // echo $_SERVER['QUERY_STRING'];
// parse_str($_SERVER['QUERY_STRING'], $_GET);
// if (empty($_GET)) {
//     if ($DEBUG) echo "<p>Not picking up the GET.</p>";
//     exit;
// }
// if ($DEBUG) echo "<p>Got a GET.</p>";
// print_r($_GET);


    parse_str($_SERVER['QUERY_STRING'], $_GET);
    echo $_GET['a'];

?>