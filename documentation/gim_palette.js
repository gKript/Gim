/*
	gim_palette.js  -  Gim documentation: the palette selector in the header.

	Two palettes, both dark, the same ones as the gKript site:
	  "modern"  gKript modern, dark grey (default)
	  "blue"    gKript blue, navy and gold

	The choice is stored in the browser (localStorage) and restored at the next visit.
	The palette itself is defined in gim_modern.css, on html[data-palette].
*/
(function () {
	var KEY = 'gim-doc-palette';
	var DEFAULT = 'modern';

	function saved() {
		try {
			var v = localStorage.getItem(KEY);
			if (v === 'blue' || v === 'modern')
				return v;
		} catch (e) {}
		return DEFAULT;
	}

	function apply(palette) {
		document.documentElement.setAttribute('data-palette', palette);
		var buttons = document.querySelectorAll('#gim-palette button');
		for (var i = 0; i < buttons.length; i++)
			buttons[i].setAttribute('aria-pressed', buttons[i].getAttribute('data-palette') === palette ? 'true' : 'false');
		try {
			localStorage.setItem(KEY, palette);
		} catch (e) {}
	}

	function build() {
		var anchor = document.getElementById('titlearea');
		if (!anchor)
			return;
		var box = document.createElement('div');
		box.id = 'gim-palette';
		box.innerHTML = '<button type="button" data-palette="modern" title="gKript modern">modern</button>' +
		                '<button type="button" data-palette="blue" title="gKript blue">blue</button>';
		anchor.appendChild(box);
		box.addEventListener('click', function (ev) {
			var b = ev.target.closest ? ev.target.closest('button') : null;
			if (b)
				apply(b.getAttribute('data-palette'));
		});
		apply(saved());
	}

	/* set the palette as soon as the script runs, then add the selector when the header exists */
	document.documentElement.setAttribute('data-palette', saved());
	if (document.readyState === 'loading')
		document.addEventListener('DOMContentLoaded', build);
	else
		build();
})();
