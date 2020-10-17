import clipboard

print clipboard.paste()

let text = "ABCDEFGabcdefg12345"

# copy text onto the clipboard
clipboard.copy(text)

# paste the text back from the clipboard
print clipboard.paste()
