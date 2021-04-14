<html>
<body>

<h1>Welcome to my home page!</h1>
<p>Some text.</p>
<p>Some more text.</p>
<?php require './include.php';
	echo "\n";

	$path = $_SERVER['DOCUMENT_ROOT'];
	echo $path;
	echo "\n";
	$config_path = __DIR__;
	echo $config_path;
	echo "\n";
	echo __FILE__;
?>

</body>
</html>
