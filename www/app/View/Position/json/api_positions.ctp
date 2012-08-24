<?php
if ($callback){
	echo $callback.'('.json_encode($satellites).');';
} else {
	echo json_encode($satellites);
}
?>
