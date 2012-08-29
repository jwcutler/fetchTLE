<style type="text/css">
.list_container {
    width: 840px;
    margin-top: 20px;
}

.source_container {
    width: 210px;
    padding: 0px 5px 5px 0px;
    box-sizing: border-box;
    -moz-box-sizing: border-box;
    -webkit-box-sizing: border-box;
    float: left;
}
</style>
<h1 class="titles">Satellite Pass Time Calculator</h1>
<p>This utility will allow you to calculate the pass times of a specified satellite over any number of ground stations. To get started, select a satellite below.</p>
<div class="list_container">
    <?php if (!empty($sources)): ?>
        <?php $source_counter = 1; ?>
        <?php foreach($sources as $source): ?>
            <div class="source_container">
                <h3 class="titles"><?php echo $source['Source']['name']; ?></h3>
                <div style="font-style: italic;margin-bottom: 5px;"><?php echo $source['Source']['description']; ?></div>
                <?php if (!empty($source['Update'][0]['Tle'])): ?>
                    <?php foreach ($source['Update'][0]['Tle'] as $tle): ?>
                        <a href="<?php echo Router::url('/', true); ?>tools/passes/<?php echo rawurlencode($tle['name']); ?>" class="link"><?php echo $tle['name']; ?></a><br />
                    <?php endforeach; ?>
                <?php else: ?>
                    <div style="font-style: italic;margin-bottom: 5px;"><?php echo $source['Source']['description']; ?></div>
                    <span style="font-style: italic;">This source has not been updated. No TLEs present.</span>
                <?php endif; ?>
            </div>
            <?php if ($source_counter == 4): ?>
                <div style="clear: both;"></div>
                <?php $source_counter = 1; ?>
            <?php else: ?>
                <?php $source_counter++; ?>
            <?php endif; ?>
        <?php endforeach; ?>
    <?php else: ?>
        <span style="font-style: italic;">No TLE sources are currently configured. Please try again later once some sources have been added.</span>
    <?php endif; ?>
</div>
