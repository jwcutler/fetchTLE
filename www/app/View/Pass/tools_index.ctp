<?php echo $this->Html->script('chosen.jquery.min.js'); ?>
<?php echo $this->Html->css('chosen.css'); ?>
<script type="text/javascript">
$(document).ready(function(){
  // Initialize the select box
  $("#tle_list").chosen({no_results_text: "No Satellites Found"});
  
  // Redirect the user if they select a satellite
  $("#tle_list").on('change', function(){
    satellite_location = $(this).val();
    
    window.location = satellite_location;
  });
});
</script>
<style type="text/css">
.list_container {
    width: 840px;
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
<p>This utility will allow you to calculate the pass times of a specified satellite over any number of ground stations. To get started, use the select box below to search for the satellite of interest or manually select it using the lists.</p>
<div style="margin: 20px 0px 10px 0px;">
  <select name="tle_list" id="tle_list" style="width: 300px;" data-placeholder="Select A Satellite">
    <option></option>
    <?php foreach($sources as $source): ?>
      <?php if (!empty($source['Update'][0]['Tle'])): ?>
        <optgroup label="<?php echo $source['Source']['name']; ?>">
          <?php foreach ($source['Update'][0]['Tle'] as $tle): ?>
            <option value="<?php echo Router::url('/', true); ?>tools/passes/<?php echo rawurlencode($tle['name']); ?>"><?php echo $tle['name']; ?></option>
          <?php endforeach; ?>
        </optgroup>
      <?php endif; ?>
    <?php endforeach; ?>
  </select>
</div>
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
