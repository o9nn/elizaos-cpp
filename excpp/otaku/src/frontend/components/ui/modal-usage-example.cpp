#include "modal-usage-example.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ExampleSimpleModal() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { showModal, hideModal } = useModal();
    const auto modalId = 'simple-modal';

    const auto handleOpenModal = [&]() {;
        showModal(;
        <div className="space-y-4">;
        <h3 className="text-lg font-semibold">Simple Modal Example</h3>;
        <p className="text-muted-foreground">;
        This is a simple modal without using createPortal!;
        </p>;
        <Button onClick={() => hideModal(modalId)}>;
        Close;
        </Button>;
        </div>,
        modalId,
        {
            closeOnBackdropClick: true,
            closeOnEsc: true,
            showCloseButton: true,
        }
        );
        };

        return (;
        <Button onClick={handleOpenModal}>;
        Open Simple Modal;
        </Button>;
        );

}

void ExampleFormModal() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { showModal, hideModal } = useModal();
    const auto modalId = 'form-modal';

    const auto handleOpenFormModal = [&]() {;
        // You can define the form content inline or as a separate component
        const auto FormContent = [&]() {;
            const auto [name, setName] = useState('');
            const auto [email, setEmail] = useState('');

            const auto handleSubmit = [&]() {;
                std::cout << 'Form submitted:' << { name, email } << std::endl;
                // Do something with the data
                hideModal(modalId);
                };

                return (;
                <div className="space-y-4">;
                <div className="flex items-center justify-between">;
                <h3 className="text-lg font-semibold">Form Example</h3>;
                </div>;

                <div className="space-y-2">;
                <label className="text-sm font-medium">Name</label>;
                <Input;
                type="text";
                placeholder="Enter your name";
            value={name}
        onChange={(e) => setName(e.target.value)}
        />;
        </div>;

        <div className="space-y-2">;
        <label className="text-sm font-medium">Email</label>;
        <Input;
        type="email";
        placeholder="Enter your email";
    value={email}
    onChange={(e) => setEmail(e.target.value)}
    />;
    </div>;

    <div className="flex gap-2">;
    <Button;
    onClick={() => hideModal(modalId)}
    variant="outline";
    className="flex-1";
    >;
    Cancel;
    </Button>;
    <Button;
    onClick={handleSubmit}
    className="flex-1";
    disabled={!name || !email}
    >;
    Submit;
    </Button>;
    </div>;
    </div>;
    );
    };

    showModal(;
    <FormContent />,
    modalId,
    {
        closeOnBackdropClick: false, // Don't close on backdrop click for forms
        closeOnEsc: true,
        showCloseButton: true,
    }
    );
    };

    return (;
    <Button onClick={handleOpenFormModal}>;
    Open Form Modal;
    </Button>;
    );

}

void ExampleConfirmationModal() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { showModal, hideModal } = useModal();
    const auto modalId = 'confirm-modal';

    const auto handleDeleteAction = [&]() {;
        showModal(;
        <div className="space-y-4">;
        <h3 className="text-lg font-semibold text-destructive">;
        Confirm Delete;
        </h3>;
        <p className="text-muted-foreground">;
        Are you sure you want to delete this item? This action cannot be undone.;
        </p>;
        <div className="flex gap-2">;
        <Button;
    onClick={() => hideModal(modalId)}
    variant="outline";
    className="flex-1";
    >;
    Cancel;
    </Button>;
    <Button;
    onClick={() => {
        std::cout << 'Item deleted!' << std::endl;
        hideModal(modalId);
    }}
    variant="destructive";
    className="flex-1";
    >;
    Delete;
    </Button>;
    </div>;
    </div>,
    modalId,
    {
        closeOnBackdropClick: true,
        closeOnEsc: true,
        showCloseButton: false, // Hide close button for confirmation modals
    }
    );
    };

    return (;
    <Button onClick={handleDeleteAction} variant="destructive">;
    Delete Item;
    </Button>;
    );

}

void ExampleCustomStyledModal() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { showModal, hideModal } = useModal();
    const auto modalId = 'custom-modal';

    const auto handleOpenCustomModal = [&]() {;
        showModal(;
        <div className="space-y-4">;
        <h3 className="text-lg font-semibold">Custom Styled Modal</h3>;
        <p className="text-muted-foreground">;
        This modal has custom width and styling!;
        </p>;
        <Button onClick={() => hideModal(modalId)}>;
        Close;
        </Button>;
        </div>,
        modalId,
        {
            closeOnBackdropClick: true,
            closeOnEsc: true,
            showCloseButton: true,
            className: 'max-w-2xl', // Custom width
        }
        );
        };

        return (;
        <Button onClick={handleOpenCustomModal}>;
        Open Custom Styled Modal;
        </Button>;
        );

}

void SendModal(auto onClose, auto tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    *   if (!isOpen) return null;
    *
    *   return createPortal(
    *     <div className="fixed inset-0 z-[100]...">
    *       // Modal content here
    *     </div>,
    *     document.body
    *   );
    *
}

void MyComponent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    *   const { showModal } = useModal();
    *
    *   const handleOpenSendModal = () => {
    *     showModal(
    *       <SendModalContent
    *         tokens={tokens}
    *         userId={userId}
    *         onSuccess={handleSuccess}
    *       />,
    *       'send-modal',
    *       { closeOnBackdropClick: true }
    *     );
    *   };
    *
    *   return <Button onClick={handleOpenSendModal}>Send</Button>;
    *
}

void ModalExamplesDemo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="p-8 space-y-4">;
    <h2 className="text-2xl font-bold mb-6">Modal System Examples</h2>;

    <div className="space-y-2">;
    <h3 className="text-lg font-semibold">Simple Modal</h3>;
    <ExampleSimpleModal />;
    </div>;

    <div className="space-y-2">;
    <h3 className="text-lg font-semibold">Form Modal</h3>;
    <ExampleFormModal />;
    </div>;

    <div className="space-y-2">;
    <h3 className="text-lg font-semibold">Confirmation Modal</h3>;
    <ExampleConfirmationModal />;
    </div>;

    <div className="space-y-2">;
    <h3 className="text-lg font-semibold">Custom Styled Modal</h3>;
    <ExampleCustomStyledModal />;
    </div>;
    </div>;
    );

}

} // namespace elizaos
