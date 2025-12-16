#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import React, { useState } from 'react';
;
;
;
;
;

const CharacterForm: React.FC = () => {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();
  const isEditing = id !== 'new';

  // Initial state - in a real app, you would fetch this data for editing
  const [formData, setFormData] = useState({
    name: isEditing ? `Character ${id}` : '',
    description: isEditing ? 'This is a character description.' : '',
    traits: isEditing ? 'Friendly, Intelligent, Curious' : '',
  });

  const handleChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
    const { name, value } = e.target;
    setFormData((prev) => ({ ...prev, [name]: value }));
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    // In a real app, you would save the data to an API here
    console.log('Form submitted:', formData);

    // Navigate back to the character list
    navigate('/characters');
  };

  return (
    <div className="p-4">
      <h1 className="text-2xl font-bold mb-4">
        {isEditing ? `Edit Character: ${formData.name}` : 'Create New Character'}
      </h1>

      <form onSubmit={handleSubmit} className="space-y-4">
        <div className="space-y-2">
          <Label htmlFor="name">Name</Label>
          <Input id="name" name="name" value={formData.name} onChange={handleChange} required />
        </div>

        <div className="space-y-2">
          <Label htmlFor="description">Description</Label>
          <Textarea
            id="description"
            name="description"
            value={formData.description}
            onChange={handleChange}
            rows={4}
          />
        </div>

        <div className="space-y-2">
          <Label htmlFor="traits">Traits (comma separated)</Label>
          <Input
            id="traits"
            name="traits"
            value={formData.traits}
            onChange={handleChange}
            placeholder="Friendly, Intelligent, Curious"
          />
        </div>

        <div className="flex gap-2">
          <Button type="submit">{isEditing ? 'Update Character' : 'Create Character'}</Button>
          <Button type="button" variant="outline" onClick={() => navigate('/characters')}>
            Cancel
          </Button>
        </div>
      </form>
    </div>
  );
};

default CharacterForm;

} // namespace elizaos
