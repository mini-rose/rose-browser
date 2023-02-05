// Replicates the Stylus app: <https://addons.mozilla.org/en-GB/firefox/addon/styl-us/>

if (document.domain == "forum.effectivealtruism.org"){
	var styles = `
	    .Layout-main {
                margin-left: 100px;
	    }
            .SingleColumnSection-root {
                width: 1000px !important;
                max-width: 1400px !important;
                padding-left: 100px !important;
            }
            .NavigationStandalone-sidebar {
                display: none;
            }
            .intercom-lightweight-app{
                display: none;
            }
  `
	  var styleSheet = document.createElement('style')
	  styleSheet.innerText = styles
	  document.head.appendChild(styleSheet)
	  console.log('Style changed')
}

document.body.style.visibility = "visible"
