<style type="text/css">
.nav-list a {
    text-decoration: none;
}
.nav-list > li > a {
    color: #0088CC;
}
.nav-list > li > a:hover {
    color: #005580;
}
</style>
<div class="page_title" style="margin-bottom: 20px;"><?php echo Configure::read('Website.name'); ?> Administration Panel</div>
<div id="admin_left_column">
    <div id="admin_nav_menu">
        <ul class="nav nav-list">
            <li class="nav-header">
                <?php echo Configure::read('Website.name'); ?> Settings
            </li>
            <li <?php if($this->params['controller']=='panel'){echo "class=\"active\"";} ?>>
                <a href="/admin"><?php echo Configure::read('Website.name'); ?> Status</a>
            </li>
            <li <?php if($this->params['controller']=='source'){echo "class=\"active\"";} ?>>
                <a href="/admin/source">TLE Source Management</a>
            </li>
            <li class="divider"></li>
            <li>
                <a href="/admin/panel/logout">Logout</a>
            </li>
        </ul>
    </div>
</div>
<div id="admin_right_column">
    <?php echo $this->fetch('panel_content'); ?>
</div>

