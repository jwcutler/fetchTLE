<?php if (!is_array($passes)): ?>
[ERROR] <?php echo $passes; ?>
<?php else: ?>
<?php foreach($passes as $pass): ?>
<?php echo $pass['satellite_name'].'#'.$pass['orbit_number'].'#'.$pass['aos'].'#'.$pass['aos_az'].'#'.$pass['mel'].'#'.$pass['mel_az'].'#'.$pass['los'].'#'.$pass['los_az'].'#'.$pass['duration'].'#'.$pass['peak_elevation'].'#'.$pass['acceptable'].'#'.$pass['ground_station']."\n"; ?>
<?php endforeach; ?>
<?php endif; ?>
