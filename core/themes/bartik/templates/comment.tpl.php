<?php

/**
 * @file
 * Bartik's theme implementation for comments.
 *
 * Available variables:
 * - $author: Comment author. Can be a link or plain text.
 * - $content: An array of comment items. Use render($content) to print them
 *   all, or print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $created: Formatted date and time for when the comment was created.
 *   Preprocess functions can reformat it by calling format_date() with the
 *   desired parameters on the $comment->created variable.
 * - $changed: Formatted date and time for when the comment was last changed.
 *   Preprocess functions can reformat it by calling format_date() with the
 *   desired parameters on the $comment->changed variable.
 * - $new: New comment marker.
 * - $permalink: Comment permalink.
 * - $submitted: Submission information created from $author and $created
 *   during template_preprocess_comment().
 * - $signature: Authors signature.
 * - $status: Comment status. Possible values are:
 *   unpublished, published, or preview.
 * - $title: Linked title.
 * - $attributes: An instance of Attributes class that can be manipulated as an
 *    array and printed as a string.
 *    It includes the 'class' information, which includes:
 *   - comment: The current template type; e.g., 'theming hook'.
 *   - by-anonymous: Comment by an unregistered user.
 *   - by-node-author: Comment by the author of the parent node.
 *   - preview: When previewing a new or edited comment.
 *   The following applies only to viewers who are registered users:
 *   - unpublished: An unpublished comment visible only to administrators.
 *   - by-viewer: Comment by the user currently viewing the page.
 *   - new: New comment since the last visit.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * These variables are provided to give context about the parent comment (if
 * any):
 * - $comment_parent: Full parent comment object (if any).
 * - $parent_author: Equivalent to $author for the parent comment.
 * - $parent_created: Equivalent to $created for the parent comment.
 * - $parent_changed: Equivalent to $changed for the parent comment.
 * - $parent_title: Equivalent to $title for the parent comment.
 * - $parent_permalink: Equivalent to $permalink for the parent comment.
 * - $parent: A text string of parent comment submission information created
 *   from $parent_author and $parent_created during
 *   template_preprocess_comment(). This information is presented to help
 *   screen readers follow lengthy discussion threads. You can hide this from
 *   sighted users using the class element-invisible.
 *
 * These two variables are provided for context:
 * - $comment: Full comment object.
 * - $node: Node entity the comments are attached to.
 *
 * @see template_preprocess()
 * @see template_preprocess_comment()
 * @see template_process()
 * @see theme_comment()
 *
 * @ingroup themeable
 */
?>
<article class="<?php print $attributes['class']; ?> clearfix"<?php print $attributes; ?> role="article">

  <header class="comment-header">

    <div class="attribution">
      <?php print render($user_picture); ?>

      <div class="submitted">
        <p class="commenter-name">
          <?php print render($author); ?>
        </p>
        <p class="comment-time">
          <?php print $created; ?>
        </p>
        <p class="comment-permalink">
          <?php print $permalink; ?>
        </p>
        <?php
          // Indicate the semantic relationship between parent and child comments
          // for accessibility. The list is difficult to navigate in a screen
          // reader without this information.
          if ($parent):
        ?>
        <p class="comment-parent element-invisible">
          <?php print $parent; ?>
        </p>
        <?php endif; ?>
      </div>
    </div> <!-- /.attribution -->

    <div class="comment-text">
      <div class="comment-arrow"></div>

      <?php if ($new): ?>
        <span class="new"><?php print $new; ?></span>
      <?php endif; ?>

      <?php print render($title_prefix); ?>
      <h3<?php print $title_attributes; ?>><?php print $title; ?></h3>
      <?php print render($title_suffix); ?>
    </div> <!-- /.comment-text -->

  </header> <!-- /.comment-header -->

  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['links']);
      print render($content);
    ?>
  </div> <!-- /.content -->

  <footer class="comment-footer">
    <?php if ($signature): ?>
      <div class="user-signature clearfix">
        <?php print $signature; ?>
      </div>
    <?php endif; ?>

    <nav>
      <?php print render($content['links']); ?>
    </nav>
  </footer> <!-- /.comment-footer -->

</article>
