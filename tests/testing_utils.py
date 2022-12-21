def check_text(response, text):
    assert response.text == text, response.text
