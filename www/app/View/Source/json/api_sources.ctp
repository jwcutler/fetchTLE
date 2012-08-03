<?
if ($callback){
	echo $callback.'('.json_encode($sources).');';
} else {
	echo json_encode($sources);
}
?>
