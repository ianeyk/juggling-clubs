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

        self.pattern_names = ["Solid Color", "Pulsing Color", "Multipole", "Confetti", "Linear Rainbow", "Vertical Rainbow", "Vertical Wave", "Torch", "BPM", "Juggle"]
        self.addin_names = ["Individually Assign Colors", "Body Color:", "Ring Color:", "Sparkle Color:", "Flash Color:"]
        self.addin_colors = [     False,                      True,          True,          True,                True]
        self.addin_sliders = [    False,                      False,         False,         True,                True]

        self.current_color_number = 0
        self.current_addin_number = 0
        self.current_slider_number = 0

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
        contents = self.html_header + patterns + self.html_footer
        return contents

    def write_to_file(self, contents):
        f = open("./data/python_constructed_html.html", "w")
        f.write(contents)
        f.close()

    def run_tests(self):
        assert(self.two_digits(0) == "00")
        assert(self.two_digits(10) == "10")
        assert(self.two_digits(99) == "99")
        assert(self.two_digits(1) == "01")

        a = self.color_iterator()
        print(a.__next__())
        print(a.__next__())
        print(a.__next__())
        print(a.__next__())
        print(a.__next__())

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
        for pattern_number, pattern_name in enumerate(pattern_names):
            html += self.generate_pattern(pattern_number, pattern_name)

        return html


    def generate_pattern(self, pattern_number, pattern_label):
        suffix = self.two_digits(pattern_number) # short hand
        pat_id = f"pattern{suffix}"
        pat_name = f"p{suffix}"

        header = f"""<div class="checkbox-label">
    <input type="checkbox" class="save-cb-state" id="{pat_id}" name="{pat_name}" value=0>
    <label for="{pat_id}" class="pattern-selector" id="label-for-{pat_id}">{pattern_label}</label>"""
        footer = """</div>"""

        contents = ""
        color_iterator = self.color_iterator(0)
        addin_iterator = self.addin_iterator(0)
        slider_iterator = self.slider_iterator(0)
        for (addin_name, show_colors, show_sliders) in zip(self.addin_names, self.addin_colors, self.addin_sliders):
            contents += self.generate_color_options(suffix, addin_name, color_iterator, addin_iterator, slider_iterator, show_colors, show_sliders)

        return header + contents + footer

    def generate_color_options(self, pat_suffix, option_label, color_iterator, addin_iterator, slider_iterator, show_colors = True, show_sliders = True):
        option_number = addin_iterator.__next__()
        suffix = pat_suffix + self.two_digits(option_number)
        opt_id = f"addin{suffix}"
        opt_name = f"a{suffix}"

        html = f"""<div class="inline-option-grid-3">
            <div class="inline-option-grid-label">
                <input type="checkbox" class="save-cb-state" id="{opt_id}" name="{opt_name}" value=0>
                <label for="{opt_id}" class="inline-option-label-label" id="label-for-{opt_id}">{option_label}</label>
            </div>""" # TODO: Is this the correct place?!

        if show_colors:
            def get_col_id(n):
                return f"color{pat_suffix + self.two_digits(n)}"
            def get_col_name(n):
                return f"c{pat_suffix + self.two_digits(n)}"

            html += f"""<div class="inline-option-grid-input color-grid-container-1">
                <div class="color0"><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
                <div class="color1"><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
                <div class="color2"><input type="color" name="{get_col_name(color_iterator.__next__())}" id="{get_col_id(self.current_color_number)}" class="color-picker-small save-val-state"></div>
            </div>"""

        if show_sliders:
            def get_sli_id(n):
                return f"slider{pat_suffix + self.two_digits(n)}"
            def get_sli_name(n):
                return f"s{pat_suffix + self.two_digits(n)}"

            html += f"""<div class="inline-option-grid-slider slider-container">
                <input type="range" class="speed-selector speed-selector-small save-val-state" name="{get_sli_name(slider_iterator.__next__())}" min=0 max=100 step=1 value=20>
                <label>Speed</label>
                <input type="range" class="speed-selector speed-selector-small save-val-state" name="{get_sli_name(slider_iterator.__next__())}" min=0 max=100 step=1 value=20>
                <label>Duration</label>
            </div>"""

        html += """</div>"""

        return html























def main():
    html_constructor = HtmlConstructor()
    html_constructor.run_tests()

    contents = html_constructor.generate_html()
    html_constructor.write_to_file(contents)

main()