<?php
if(!isset($_GET['code'])){
	die();
}

$macid=$_GET['code'];
$time=time()+24*60*60;
$ip = $_SERVER['REMOTE_ADDR'];

$mysql_conf = array(
    'host'    => 'localhost:3306', 
    'db'      => 'database_wowfish', 
    'db_user' => 'user_wowfish', 
    'db_pwd'  => 'user_wowfish_pwd', 
    );

$mysqli = @new mysqli($mysql_conf['host'], $mysql_conf['db_user'], $mysql_conf['db_pwd']);
if ($mysqli->connect_errno) {
    die();
}
$mysqli->query("set names 'utf8';");
$select_db = $mysqli->select_db($mysql_conf['db']);
if (!$select_db) {
    die();
}

$sql = "select time from regdata where macid = '$macid';";
$res = $mysqli->query($sql);


if (!$res) {
    die();
}

if ($row = $res->fetch_assoc()) {
    echo time().','.$row['time'];
}else{
    $sql = "INSERT INTO regdata (macid,time,num,ip) VALUES('$macid','$time','0','$ip');";
    $res = $mysqli->query($sql);
    echo time().','.$time;
}


$res->free();
$mysqli->close();

?>
