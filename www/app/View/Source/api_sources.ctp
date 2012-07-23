<?php if (empty($sources)): ?>
[ERROR] No TLEs could be found.
<?php else: ?>
<?php foreach($sources as $source): ?>
<?php echo $source['name']."\n"; ?>
<?php echo $source['raw_l1']."\n"; ?>
<?php echo $source['raw_l2']."\n"; ?>
<?php endforeach; ?>
<?php endif; ?>
