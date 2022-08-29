from bs4 import BeautifulSoup
import htmlmin

class HtmlConstructor():
    def __init__(self):
        self.html_header = """<!DOCTYPE html>
        <link rel="stylesheet" href="style.css" />
        <html>
        <body>

        <h1>Show a Color Picker</h1>

        <form action="" method="get">

            <div class="pattern-set">
                <p class="main-text">Select which patterns you would like to cycle through. If none are selected, all patterns will be used.</p>
        """
        self.html_footer = """    </div>

            <br><br>

            <input type='submit' value='Submit'>
        </form>


        </body>
        </html>
        """
        self.java_script = self.read_javascript_file()

        self.pattern_names = [
            # Pattern name      # Options available
            ("Solid Color",      "single-color"),
            ("Pulsing Color",    "single-color"),
            ("Multipole",        "double-color"),
            ("Confetti",         "single-color"),
            ("Linear Rainbow",   "no_color"),
            ("Vertical Rainbow", "no_color"),
            ("Vertical Wave",    "double-color"),
            ("Torch",            "double-color"),
            ("BPM",              "no_color"),
            ("Juggle",           "no_color"),
        ]


        self.pattern_options_list = {
            "full-list": [
                "Individually Assign Colors",
                "Body Color:",
                "Secondary Color:",
                "Ring Color:",
                "Sparkle Color:",
                "Flash Color:",
                "Pattern Speed:",
                "Color Cycle Speed:",
            ],
            "double-color": [
                "Individually Assign Colors",
                "Body Color:",
                "Secondary Color:",
                "Ring Color:",
                "Sparkle Color:",
                "Flash Color:",
                "Pattern Speed:",
                "Color Cycle Speed:",
            ],
            "single-color": [
                "Individually Assign Colors",
                "Body Color:",
                "Ring Color:",
                "Sparkle Color:",
                "Flash Color:",
                "Pattern Speed:",
                "Color Cycle Speed:",
            ],
            "no_color": [
                "Sparkle Color:",
                "Flash Color:",
                "Pattern Speed:",
            ],
        }

        self.pattern_options_definitions = {
            # Add-in name           # show colors?  # sliders?  # sliders-only
            "Individually Assign Colors": (False,     False,     False),
            "Body Color:":                (True,      False,     False),
            "Secondary Color:":           (True,      False,     False),
            "Ring Color:":                (True,      False,     False),
            "Sparkle Color:":             (True,      True,      False),
            "Flash Color:":               (True,      True,      False),
            "Pattern Speed:":             (False,     False,     True ),
            "Color Cycle Speed:":         (False,     False,     True ),
        }

        self.current_color_number = 0
        self.current_addin_number = 0
        self.current_slider_number = 0
        self.addin_number_of_color_vis_controller = self.pattern_options_list["full-list"].index("Individually Assign Colors")

    def color_iterator(self, start = 0):
        current_color_number = start
        while True:
            self.current_color_number = current_color_number
            yield current_color_number
            current_color_number += 1

    def addin_iterator(self, start = 0):
        current_addin_number = start
        while True:
            # same as color_iterator but with a different static variable
            self.current_addin_number = current_addin_number
            yield current_addin_number
            current_addin_number += 1

    def slider_iterator(self, start = 0):
        current_slider_number = start
        while True:
            # same as color_iterator but with a different static variable
            self.current_slider_number = current_slider_number
            yield current_slider_number
            current_slider_number += 1

    def generate_html(self):
        patterns = self.generate_patterns(self.pattern_names)
        contents = self.html_header + patterns + self.html_footer + self.java_script
        return contents

    def write_to_file(self, contents):
        soup = BeautifulSoup(contents, 'html.parser')
        beautiful = soup.prettify()
        f = open("./data/python_constructed_html.html", "w")
        f.write(beautiful)
        f.close()

        minified = htmlmin.minify(contents)
        f2 = open("./data/python_minified_html.html", "w")
        f2.write(minified)
        f2.close()

    def read_javascript_file(self):
        f = open("./data/javascript_template.html", "r")
        contents = f.read()
        f.close()
        return contents

    def run_tests(self):
        assert(self.two_digits(0) == "00")
        assert(self.two_digits(10) == "10")
        assert(self.two_digits(99) == "99")
        assert(self.two_digits(1) == "01")

        print("All tests passed!")

    def two_digits(self, n: int) -> str:
        assert(n >= 0)
        assert(n < 100)
        assert(isinstance(n, int))
        if n >= 10:
            return str(n)
        # else:
        return "0" + str(n)

    def generate_patterns(self, pattern_names):
        html = ""
        for pattern_number, (pattern_name, pattern_options) in enumerate(pattern_names):
            html += self.generate_pattern(pattern_number, pattern_name, pattern_options)

        return html


    def generate_pattern(self, pattern_number, pattern_label, pattern_options):
        suffix = self.two_digits(pattern_number) # short hand
        pat_id = f"pattern{suffix}"
        pat_name = f"p{suffix}"

        header = f"""<div class="checkbox-label">
    <input type="checkbox" class="save-cb-state" id="{pat_id}" name="{pat_name}" value=0>
    <label for="{pat_id}" class="pattern-selector" id="label-for-{pat_id}">{pattern_label}</label>
    <div class="pattern-details hide-by-default" id="details-for-{pat_id}">
    """
        footer = """</div></div>"""

        contents = ""
        color_iterator = self.color_iterator(0)
        addin_iterator = self.addin_iterator(0)
        slider_iterator = self.slider_iterator(0)


        for options_key in self.pattern_options_list["full-list"]: # search the full list
            ok_to_append = options_key in self.pattern_options_list[pattern_options]
            show_colors, show_sliders, slider_only = self.pattern_options_definitions[options_key]
            # (addin_name, show_colors, show_sliders, slider_only)
            # (show_colors, show_sliders, slider_only)
            if slider_only:
                if ok_to_append:
                    contents += self.generate_slider_options(suffix, options_key, addin_iterator, slider_iterator)
                else:
                    self.generate_slider_options(suffix, options_key, addin_iterator, slider_iterator) # without appending
            else:
                if ok_to_append:
                    contents += self.generate_color_options(suffix, options_key, color_iterator, addin_iterator, slider_iterator, show_colors, show_sliders)
                else: self.generate_color_options(suffix, options_key, color_iterator, addin_iterator, slider_iterator, show_colors, show_sliders) # without appending

        return header + contents + footer

    def generate_color_options(self, pat_suffix, option_label, color_iterator, addin_iterator, slider_iterator, show_colors = True, show_sliders = True):
        option_number = addin_iterator.__next__()
        suffix = pat_suffix + self.two_digits(option_number)
        opt_id = f"addin{suffix}"
        opt_name = f"a{suffix}"

        details_opt_suffix = pat_suffix + self.two_digits(self.addin_number_of_color_vis_controller)
        details_opt_id = f"addin{details_opt_suffix}"

        header = f"""<div class="inline-option-grid-3">
            <div class="inline-option-grid-label">
                <input type="checkbox" class="save-cb-state" id="{opt_id}" name="{opt_name}" value=0>
                <label for="{opt_id}" class="inline-option-label-label" id="label-for-{opt_id}">{option_label}</label>
            </div>"""

        footer = """</div>"""

        contents = ""

        def get_col_id(n):
            return f"color{pat_suffix + self.two_digits(n)}"
        def get_col_name(n):
            return f"c{pat_suffix + self.two_digits(n)}"

        new_contents = f"""<div class="inline-option-grid-input color-grid-container-1">
            <div class="color0"                                                    ><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
            <div class="color1 hide-by-default" name="details-for-{details_opt_id}"><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
            <div class="color2 hide-by-default" name="details-for-{details_opt_id}"><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
        </div>"""

        if show_colors:
            contents += new_contents

        elif option_number == self.addin_number_of_color_vis_controller:
            new_contents = f"""<div class="inline-option-grid-input color-grid-container-1">
                <div class="color0 club-number-label"                                    >Club 1</div>
                <div class="color1 club-number-label" name="details-for-{details_opt_id}">Club 2</div>
                <div class="color2 club-number-label" name="details-for-{details_opt_id}">Club 3</div>
            </div>"""
            # add 'hide-by-default' to the list of class names if you want them to disappear
            contents += new_contents

        def get_sli_id(n):
            return f"slider{pat_suffix + self.two_digits(n)}"
        def get_sli_name(n):
            return f"s{pat_suffix + self.two_digits(n)}"

        new_contents = f"""<div class="inline-option-grid-slider slider-container">
            <input type="range" class="speed-selector speed-selector-small save-val-state" name="{get_sli_name(slider_iterator.__next__())}" min=0 max=100 step=1 value=20>
            <label>Speed</label>
            <input type="range" class="speed-selector speed-selector-small save-val-state" name="{get_sli_name(slider_iterator.__next__())}" min=0 max=100 step=1 value=20>
            <label>Duration</label>
        </div>"""

        if show_sliders:
            contents += new_contents

        return header + contents + footer

    def generate_slider_options(self, pat_suffix, option_label, addin_iterator, slider_iterator):
        option_number = addin_iterator.__next__()
        suffix = pat_suffix + self.two_digits(option_number)
        opt_id = f"addin{suffix}"
        opt_name = f"a{suffix}"

        def get_sli_id(n):
            return f"slider{pat_suffix + self.two_digits(n)}"
        def get_sli_name(n):
            return f"s{pat_suffix + self.two_digits(n)}"

        return f"""<div class="inline-option-grid-2">
                    <div class="inline-option-grid-label">{option_label}</div>
                    <div class="inline-option-grid-slider slider-container">
                        <input type="range" class="speed-selector speed-selector-medium save-val-state"  name="{get_sli_name(slider_iterator.__next__())}" min=0 max=100 step=1 value=20 oninput="document.getElementById('label-for-{get_sli_id(self.current_slider_number)}').innerHTML = '(' + (this.value == 0 ? 'frozen' : ((Math.abs((10.001 / this.value)).toFixed(1) + 'x'))) + ')'">
                        <label id="label-for-{get_sli_id(self.current_slider_number)}">()</label>
                    </div>
                </div>"""



def main():
    html_constructor = HtmlConstructor()
    html_constructor.run_tests()

    contents = html_constructor.generate_html()
    html_constructor.write_to_file(contents)

main()