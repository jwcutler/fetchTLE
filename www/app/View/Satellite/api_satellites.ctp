<?php if (empty($satellites)): ?>
[ERROR] No TLEs could be found.
<?php else: ?>
<?php foreach($satellites as $satellite): ?>
<?php echo $satellite['name']."\n"; ?>
<?php echo $satellite['raw_l1']."\n"; ?>
<?php echo $satellite['raw_l2']."\n"; ?>
<?php endforeach; ?>
<?php endif; ?>
