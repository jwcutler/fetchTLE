<?php
if ($callback){
	echo $callback.'('.json_encode($passes).');';
} else {
	echo json_encode($passes);
}
?>
