/**
 * @file
 * Attaches behaviors for the Contextual module's edit toolbar tab.
 */

(function ($, Drupal, Backbone) {

"use strict";

var options = {
  strings: {
    tabbingReleased: Drupal.t('Tabbing is no longer constrained by the Contextual module.'),
    tabbingConstrained: Drupal.t('Tabbing is constrained to a set of @contextualsCount and the Edit mode toggle.'),
    pressEsc: Drupal.t('Press the esc key to exit.'),
    contextualsCount: {
      singular: '@count contextual link',
      plural: '@count contextual links'
    }
  }
};

/**
 * Initializes a contextual link: updates its DOM, sets up model and views
 *
 * @param DOM links
 *   A contextual links DOM element as rendered by the server.
 */
function initContextualToolbar (context) {
  var contextualToolbar = Drupal.contextualToolbar;
  var model = contextualToolbar.model = new contextualToolbar.Model({
    isViewing: true,
    isVisible: false
  });

  var viewOptions = $.extend({
    el: $('.js .toolbar .bar .contextual-toolbar-tab'),
    model: model
  }, options);
  new contextualToolbar.VisualView(viewOptions);
  new contextualToolbar.AuralView(viewOptions);

  // Update the model based on overlay events.
  $(document).on({
    'drupalOverlayOpen.contextualToolbar': function () {
      model.set('isVisible', false);
    },
    'drupalOverlayClose.contextualToolbar': function () {
      model.set('isVisible', true);
    }
  });

  // Show the edit tab while there's >=1 contextual link.
  var collection = Drupal.contextual.collection;
  var updateVisibility = function () {
    model.set('isVisible', collection.length > 0);
  };
  collection.on('reset remove add', updateVisibility);
  updateVisibility();

  // Whenever edit mode is toggled, update all contextual links.
  model.on('change:isViewing', function() {
    collection.each(function (contextualModel) {
      contextualModel.set('isLocked', !model.get('isViewing'));
    });
  });

  // Checks whether localStorage indicates we should start in edit mode
  // rather than view mode.
  // @see Drupal.contextualToolbar.VisualView.persist()
  if (localStorage.getItem('Drupal.contextualToolbar.isViewing') === 'false') {
    model.set('isViewing', false);
  }
}

/**
 * Attaches contextual's edit toolbar tab behavior.
 */
Drupal.behaviors.contextualToolbar = {
  attach: function (context) {
    if ($('body').once('contextualToolbar-init').length) {
      initContextualToolbar(context);
    }
  }
};

/**
 * Model and View definitions.
 */
Drupal.contextualToolbar = {
  // The Drupal.contextualToolbar.Model instance.
  model: null,

  /**
   * Models the state of the edit mode toggle.
   */
  Model: Backbone.Model.extend({
    defaults: {
      // Indicates whether the toggle is currently in "view" or "edit" mode.
      isViewing: true,
      // Indicates whether the toggle should be visible or hidden.
      isVisible: false,
      // The set of elements that can be reached via the tab key when edit mode
      // is enabled.
      tabbingContext: null
    }
  }),

  /**
   * Renders the visual view of the edit mode toggle. Listens to mouse & touch.
   *
   * Handles edit mode toggle interactions.
   */
  VisualView: Backbone.View.extend({
    events: function () {
      // Prevents delay and simulated mouse events.
      var touchEndToClick = function (event) {
        event.preventDefault();
        event.target.click();
      };

      return {
        'click': function () {
          this.model.set('isViewing', !this.model.get('isViewing'));
        },
        'touchend': touchEndToClick
      };
    },

    /**
     * {@inheritdoc}
     */
    initialize: function () {
      this.model.on('change', this.render, this);
      this.model.on('change:isViewing', this.persist, this);
    },

    /**
     * {@inheritdoc}
     */
    render: function () {
      // Render the visibility.
      this.$el.toggleClass('element-hidden', !this.model.get('isVisible'));
      // Render the state.
      this.$el.find('button').toggleClass('active', !this.model.get('isViewing'));

      return this;
    },

    /**
     * Model change handler; persists the isViewing value to localStorage.
     *
     * isViewing === true is the default, so only stores in localStorage when
     * it's not the default value (i.e. false).
     *
     * @param Drupal.contextualToolbar.Model model
     *   A Drupal.contextualToolbar.Model model.
     * @param bool isViewing
     *   The value of the isViewing attribute in the model.
     */
    persist: function (model, isViewing) {
      if (!isViewing) {
        localStorage.setItem('Drupal.contextualToolbar.isViewing', 'false');
      }
      else {
        localStorage.removeItem('Drupal.contextualToolbar.isViewing');
      }
    }
  }),

  /**
   * Renders the aural view of the edit mode toggle (i.e.screen reader support).
   */
  AuralView: Backbone.View.extend({
    // Tracks whether the tabbing constraint announcement has been read once yet.
    announcedOnce: false,

    /*
     * {@inheritdoc}
     */
    initialize: function () {
      this.model.on('change', this.render, this);
      this.model.on('change:isViewing', this.manageTabbing, this);

      $(document).on('keyup', _.bind(this.onKeypress, this));
    },

    /**
     * {@inheritdoc}
     */
    render: function () {
      // Render the state.
      this.$el.find('button').attr('aria-pressed', !this.model.get('isViewing'));

      return this;
    },

    /**
     * Limits tabbing to the contextual links and edit mode toolbar tab.
     *
     * @param Drupal.contextualToolbar.Model model
     *   A Drupal.contextualToolbar.Model model.
     * @param bool isViewing
     *   The value of the isViewing attribute in the model.
     */
    manageTabbing: function (model, isViewing, options) {
      var tabbingContext = this.model.get('tabbingContext');
      // Always release an existing tabbing context.
      if (tabbingContext) {
        tabbingContext.release();
        Drupal.announce(this.options.strings.tabbingReleased);
      }
      // Create a new tabbing context when edit mode is enabled.
      if (!isViewing) {
        tabbingContext = Drupal.tabbingManager.constrain($('.contextual-toolbar-tab, .contextual'));
        this.model.set('tabbingContext', tabbingContext);
        this.announceTabbingConstraint();
        this.announcedOnce = true;
      }
    },

    /**
     * Announces the current tabbing constraint.
     */
    announceTabbingConstraint: function () {
      var strings = this.options.strings;
      Drupal.announce(Drupal.t(strings.tabbingConstrained, {
        '@contextualsCount': Drupal.formatPlural(Drupal.contextual.collection.length, strings.contextualsCount.singular, strings.contextualsCount.plural)
      }));
      Drupal.announce(strings.pressEsc);
    },

    /**
     * Responds to esc and tab key press events.
     *
     * @param jQuery.Event event
     */
    onKeypress: function (event) {
      // The first tab key press is tracked so that an annoucement about tabbing
      // constraints can be raised if edit mode is enabled when the page is
      // loaded.
      if (!this.announcedOnce && event.keyCode === 9 && !this.model.get('isViewing')) {
        this.announceTabbingConstraint();
        // Set announce to true so that this conditional block won't run again.
        this.announcedOnce = true;
      }
      // Respond to the ESC key. Exit out of edit mode.
      if (event.keyCode === 27) {
        this.model.set('isViewing', true);
      }
    }
  })
};

})(jQuery, Drupal, Backbone);
