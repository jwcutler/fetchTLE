<?php if (empty($satellites)): ?>
[ERROR] No satellite positions were calculated successfully. 
<?php else: ?>
<?php foreach($satellites as $satellite): ?>
<?php echo $satellite['name'].':'.$satellite['timestamp'].':'.$satellite['latitude'].':'.$satellite['longitude'].':'.$satellite['altitude']."\n"; ?>
<?php endforeach; ?>
<?php endif; ?>
