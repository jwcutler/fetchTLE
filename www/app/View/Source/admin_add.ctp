<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<script type="text/javascript">
$(document).ready(function(){    
    // Form validation
    var container = $("div.form_errors");
    $("#new_source_form").validate({
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
<h3>Add A New TLE Source</h3>
Use the form below to create a new TLE source.
<div class="form_errors">
    <ul>
    </ul>
</div>
<div style="width: 70%; margin-top: 10px;">
    <form action="/admin/source/add" method="POST" class="form-horizontal" id="new_source_form">
        <div class="control-group">
            <label class="control-label" for="source_name">Source Name</label>
            <div class="controls">
                <input type="text" name="source_name" id="source_name" />
                <p class="help-block">The name the source will be represented by in <?php echo Configure::read('Website.name'); ?>.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_url">Source URL</label>
            <div class="controls">
                <input type="text" name="source_url" id="source_url" />
                <p class="help-block">The URL of the TLE source. This URL must be a standard TLE file.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_description">Description</label>
            <div class="controls">
                <textarea name="source_description" id="source_description"></textarea>
            </div>
        </div>
        <button type="submit" class="btn btn-success">Add Source</button>
    </form>
</div>
<?php
$this->end();
?>

