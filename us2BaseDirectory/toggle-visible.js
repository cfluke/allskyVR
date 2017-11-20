/* global AFRAME */

/**
 * Component that listens to an event, and toggles visiblity of the entity 
 */
AFRAME.registerComponent('toggle-visible', {
  schema: {
    on: {type: 'string'},
    target: {type: 'selector'},
  },

  init: function () {
    var data = this.data;
    var el = this.el;

    el.addEventListener(data.on, function () {
      if (data.target.getAttribute('visible') == true) {
         data.target.setAttribute('visible', false); 
      } else {
         data.target.setAttribute('visible', true); 
      }
    });
  }
});
