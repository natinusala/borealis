/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <borealis/core/activity.hpp>
#include <borealis/core/view.hpp>

namespace brls
{

// Exception thrown when using a BoundView with an unknown ID for
// the given owner.
class ViewNotFoundException : public std::exception
{
  public:
    ViewNotFoundException(View* owner, std::string searchedId);
    ViewNotFoundException(Activity* owner, std::string searchedId);

    const char* what() const noexcept;

  private:
    std::string errorMessage;
};

#define BRLS_BIND(type, name, id) brls::BoundView<type> name = brls::BoundView<type>(id, this)

// A BoundView represents a "child" view, bound to its "parent" view class.
// Dereferencing operator has been overridden to directly resolve the view by ID and forward the call to the view pointer for you.
// Allows easy access of child views inside the parent class when used with the BRLS_BIND macro.
// Does NOT do any type check - it assumes the type you give it is the same as the view you are trying to get.
template <typename T>
class BoundView
{
  public:
    BoundView(std::string id, View* owner)
        : id(id)
        , ownerView(owner)
    {
    }

    BoundView(std::string id, Activity* owner)
        : id(id)
        , ownerActivity(owner)
    {
    }

    T* getView()
    {
        this->resolve();
        return this->view;
    }

    operator T*()
    {
        return this->getView();
    }

    T* operator->()
    {
        return this->getView();
    }

    bool operator==(const T* value)
    {
        return this->getView() == value;
    }

  private:
    std::string id = "";

    T* view = nullptr;

    View* ownerView         = nullptr;
    Activity* ownerActivity = nullptr;

    /**
     * Resolve the view pointer by id.
     *
     * Not called in the constructor
     * on purpose because the owner might now be ready when
     * the BoundView is constructed, so wait for the first
     * actual call before resolving the view.
     */
    void resolve()
    {
        if (this->view)
            return;

        // Resolve by owner activity first
        if (this->ownerView)
        {
            this->view = (T*)this->ownerView->getView(this->id);

            if (!this->view)
                throw ViewNotFoundException(this->ownerView, this->id);
        }
        // Then resolve by owner view
        else if (this->ownerActivity)
        {
            this->view = (T*)this->ownerActivity->getView(this->id);

            if (!this->view)
                throw ViewNotFoundException(this->ownerActivity, this->id);
        }
        else
        {
            throw std::logic_error("No owner view or activity given to BoundView");
        }
    }
};

} // namespace brls
