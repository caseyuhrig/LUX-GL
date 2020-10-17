#pragma once

#include <string>
#include <map>
#include <memory>

#include "glfont/GLFont.h"
#include "glfont/FTLabel.h"


// MIT license for the font code https://github.com/jtberglund/GLFont/blob/master/LICENSE
// gonna re-write the code. reminder to ack. the orig. author.
// TODO Whenever the window is resized, you should update the window size of your label
// label->setWindowSize(windowWidth, windowSize);
namespace lux {

    class TextList
    {
    public:
        TextList() 
            : _x(0.0f), _y(0.0f), m_Width(0.0f), m_Height(0.0f), _y_pos(0.0f)
        {}

        TextList(float x, float y, float width, float height)
            : _x(x), _y(y), m_Width(width), m_Height(height), _y_pos(y)
        {}

        void Resize(const float width, const float height)
        {
            m_Width = width;
            m_Height = height;
            for (const auto& [id, label] : _labels)
            {
                label->setWindowSize(width, height);
                //label->setPosition(label->getX(), label->getY());
                //label->setText(label->getText());
            }
        }

        void AddText(const uint32_t fontID, const uint32_t labelID, const int fontSize, const float yOffset, const std::string& text)
        {
            const auto font = _fonts.at(fontID);
            const auto label = CreateRef<FTLabel>(font, text, _x, _y_pos, m_Width, m_Height);
            label->setColor(1.0, 1.0, 1.0, 1.0);
            label->setPixelSize(fontSize);
            _labels.emplace(labelID, label);
            _y_pos += yOffset;
            //_y_pos += font->GetFontFace()->max_advance_height;
        }

        void AddFont(const uint32_t fontID, const std::string& fontPath)
        {
            const auto font = CreateRef<GLFont>(fontPath);
            // TODO check if the font exists first
            _fonts.emplace(fontID, font);
        }

        void SetText(const uint32_t& labelID, const std::string& text) const
        {
            auto label = _labels.at(labelID);
            label->setText(text);
        }

        void Draw() const
        {
            for (const auto& [id, label] : _labels)
            {
                label->render();
            }
        }
    private:
        float _x, _y;
        float m_Width, m_Height;
        float _y_pos;
        std::unordered_map<uint32_t, Ref<GLFont>> _fonts;
        // TODO make fonts static, etc...
        // res/fonts/Inconsolata/static/InconsolataCondensed-Medium.ttf
        // res/fonts/Inconsolata/static/InconsolataCondensed-Light.ttf
        //std::vector<std::unique_ptr<FTLabel>> _labels;
        std::unordered_map<uint32_t, Ref<FTLabel>> _labels;
    };
}