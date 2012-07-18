<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<script type="text/javascript">
$(document).ready(function(){    
    // Form validation
    var container = $("div.form_errors");
    $("#edit_source_form").validate({
        errorContainer: container,
        errorLabelContainer: $("ul", container),
        errorClass: "form_error",
        wrapper: 'li',
        rules: {
            source_name: {
                required: true
            },
            source_url: {
                required: true
            }
        },
        messages: {
            source_name: {
                required: "Please enter a source name."
            },
            source_url: {
                required: "Please enter a source URL."
            }
        }
    });
});
</script>
<h3>Editing the '<?php echo $source['Source']['name']; ?>' TLE Source</h3>
Use the form below to edit the source.
<div class="form_errors">
    <ul>
    </ul>
</div>
<div style="width: 70%; margin-top: 10px;">
    <form action="/admin/source/<?php echo $source['Source']['id']; ?>/edit" method="POST" class="form-horizontal" id="edit_source_form">
        <div class="control-group">
            <label class="control-label" for="source_name">Source Name</label>
            <div class="controls">
                <input type="text" name="source_name" id="source_name" value="<?php echo $source['Source']['name']; ?>" />
                <p class="help-block">Note that if you change this any scripts that referenced this source with the API will need to be updated.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_url">Source URL</label>
            <div class="controls">
                <input type="text" name="source_url" id="source_url" value="<?php echo $source['Source']['url']; ?>" />
                <p class="help-block">This must be a standard TLE file.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_description">Source Description</label>
            <div class="controls">
                <textarea name="source_description"><?php echo $source['Source']['description']; ?></textarea>
            </div>
        </div>
        <button type="submit" class="btn btn-success">Edit Source</button>
        <a href="/admin/source/<?php echo $source['Source']['id']; ?>/delete" class="btn btn-danger">Delete Source</a>
    </form>
</div>
<?php
$this->end();
?>

