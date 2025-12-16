#include "..assets/left.png.hpp"
#include "..assets/right.png.hpp"
#include "types.hpp"
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

import React, { useState, useRef, useCallback, useEffect } from 'react';
;
;
;

const PhotoEditor: React.FC = () => {
    const [baseImage, setBaseImage] = useState<string>('');
    const [currentHatImage, setCurrentHatImage] = useState<string>(rightImage);
    const [transform, setTransform] = useState<Transform>({
        position: { x: 0, y: 0 },
        rotation: 0,
        scale: 1,
        flipX: false,
    });
    const [originalImageSize, setOriginalImageSize] = useState<{ width: number; height: number }>({ width: 0, height: 0 });
    const [status, setStatus] = useState<{ message: string; type: 'error' | 'success' } | null>(null);

    const containerRef = useRef<HTMLDivElement>(null);
    const overlayRef = useRef<HTMLDivElement>(null);
    const isDragging = useRef(false);
    const dragStart = useRef<Position>({ x: 0, y: 0 });
    const statusTimeoutRef = useRef<NodeJS.Timeout>();


    const showStatus = useCallback((message: string, type: 'error' | 'success') => {
        if (statusTimeoutRef.current) {
            clearTimeout(statusTimeoutRef.current);
        }

        setStatus({ message, type });

        statusTimeoutRef.current = setTimeout(() => {
            setStatus(null);
        }, 1000);
    }, []);

    const handleBaseImageUpload = useCallback((event: React.ChangeEvent<HTMLInputElement>) => {
        const file = event.target.files?.[0];
        if (file) {
            if (!file.type.startsWith('image/')) {
                showStatus('Please select an image file', 'error');
                return;
            }

            const reader = new FileReader();
            reader.onload = (e) => {
                const img = new Image();
                img.onload = () => {
                    setOriginalImageSize({ width: img.width, height: img.height });
                    showStatus('Image loaded successfully', 'success');
                };
                const dataUrl = e.target?.result as string;
                img.src = dataUrl;
                setBaseImage(dataUrl);
            };
            reader.readAsDataURL(file);
        }
    }, [showStatus]);

    useEffect(() => {
        return () => {
            if (statusTimeoutRef.current) {
                clearTimeout(statusTimeoutRef.current);
            }
        };
    }, []);


    const handleTouchStart = useCallback((e: React.TouchEvent) => {
        e.preventDefault();
        if (e.touches.length === 1) {
            isDragging.current = true;
            const touch = e.touches[0];
            dragStart.current = {
                x: touch.clientX - transform.position.x,
                y: touch.clientY - transform.position.y,
            };
        }
    }, [transform.position]);

    const handleTouchMove = useCallback((e: React.TouchEvent) => {
        e.preventDefault();
        if (isDragging.current && e.touches.length === 1) {
            const touch = e.touches[0];
            setTransform(prev => ({
                ...prev,
                position: {
                    x: touch.clientX - dragStart.current.x,
                    y: touch.clientY - dragStart.current.y,
                },
            }));
        }
    }, []);

    const handleTouchEnd = useCallback(() => {
        isDragging.current = false;
    }, []);

    const handleMouseDown = useCallback((e: React.MouseEvent) => {
        isDragging.current = true;
        dragStart.current = {
            x: e.clientX - transform.position.x,
            y: e.clientY - transform.position.y,
        };
    }, [transform.position]);

    const handleMouseMove = useCallback((e: React.MouseEvent) => {
        if (isDragging.current) {
            setTransform(prev => ({
                ...prev,
                position: {
                    x: e.clientX - dragStart.current.x,
                    y: e.clientY - dragStart.current.y,
                },
            }));
        }
    }, []);

    const handleMouseUp = useCallback(() => {
        isDragging.current = false;
    }, []);

    const handleRotate = useCallback((direction: 'left' | 'right') => {
        setTransform(prev => ({
            ...prev,
            rotation: prev.rotation + (direction === 'left' ? -15 : 15),
        }));
    }, []);

    const handleScale = useCallback((direction: 'up' | 'down') => {
        setTransform(prev => ({
            ...prev,
            scale: Math.max(0.1, Math.min(7, prev.scale * (direction === 'up' ? 1.1 : 0.9))),
        }));
    }, []);

    const handleFlip = useCallback(() => {
        setCurrentHatImage(prev => prev === rightImage ? leftImage : rightImage);
    }, []);

    const handleReset = useCallback(() => {
        setTransform({
            position: { x: 0, y: 0 },
            rotation: 0,
            scale: 1,
            flipX: false,
        });
    }, []);

    const handleSave = useCallback(async () => {
        if (!baseImage || !overlayRef.current || !containerRef.current) {
            showStatus('Please upload an image first', 'error');
            return;
        }

        try {
            const canvas = document.createElement('canvas');
            const ctx = canvas.getContext('2d');
            if (!ctx) {
                throw new Error('Could not get canvas context');
            }

            const baseImg = new Image();
            baseImg.src = baseImage;
            await new Promise(resolve => baseImg.onload = resolve);

            canvas.width = originalImageSize.width;
            canvas.height = originalImageSize.height;

            ctx.drawImage(baseImg, 0, 0, canvas.width, canvas.height);

            const container = containerRef.current;
            const containerRect = container.getBoundingClientRect();
            const containerAspect = containerRect.width / containerRect.height;
            const imageAspect = canvas.width / canvas.height;

            let displayedWidth = containerRect.width;
            let displayedHeight = containerRect.height;
            if (containerAspect > imageAspect) {
                displayedWidth = displayedHeight * imageAspect;
            } else {
                displayedHeight = displayedWidth / imageAspect;
            }

            const scaleX = canvas.width / displayedWidth;
            const scaleY = canvas.height / displayedHeight;

            const overlayImg = overlayRef.current.querySelector('img');
            if (overlayImg) {
                const hatImg = new Image();
                hatImg.src = currentHatImage;
                await new Promise(resolve => hatImg.onload = resolve);

                const centerX = canvas.width / 2;
                const centerY = canvas.height / 2;

                ctx.save();
                ctx.translate(centerX + (transform.position.x * scaleX), centerY + (transform.position.y * scaleY));
                ctx.rotate((transform.rotation * Math.PI) / 180);
                if (transform.flipX) {
                    ctx.scale(-1, 1);
                }
                ctx.scale(transform.scale, transform.scale);

                const overlayWidth = 100 * scaleX;
                const overlayHeight = (overlayWidth * hatImg.height) / hatImg.width;
                ctx.drawImage(hatImg, -overlayWidth / 2, -overlayHeight / 2, overlayWidth, overlayHeight);

                ctx.restore();
            }

            const link = document.createElement('a');
            link.download = 'you-are-a-partner-now.png';
            link.href = canvas.toDataURL('image/png');
            link.click();

            showStatus('Image saved successfully', 'success');
        } catch (error) {
            console.error('Save error:', error);
            showStatus('Error saving image', 'error');
        }
    }, [baseImage, transform, originalImageSize, currentHatImage, showStatus]);

    const getOverlayStyle = () => {
        return {
            transform: `translate(-50%, -50%) 
                       translate(${transform.position.x}px, ${transform.position.y}px)
                       rotate(${transform.rotation}deg)
                       scale(${transform.scale * (transform.flipX ? -1 : 1)}, ${transform.scale})`,
        };
    };

    return (
        <div className="flex flex-col items-center gap-4 p-4 w-full min-h-screen font-['Space_Grotesk'] bg-[#1a1a1a] text-[#f0f0f0] overflow-x-hidden">
            <div className="flex flex-col items-center w-full max-w-7xl mx-auto">
                <div className="text-center mb-8">
                    <h1 className="text-[#ff6b2b] text-4xl uppercase tracking-wider font-bold m-0">
                        Put on your hat
                    </h1>
                    <p className="text-[#ff8f5a] text-lg mt-2">
                        BE A PARTNER
                    </p>
                </div>

                <div className="w-full max-w-md">
                    <input
                        type="file"
                        accept="image/*"
                        onChange={handleBaseImageUpload}
                        className="w-full p-3 border-2 border-[#ff6b2b] rounded-lg bg-[#2a2a2a] text-white cursor-pointer transition-all hover:border-[#ff8f5a] hover:bg-[#3a3a3a]"
                    />
                </div>

                <div
                    ref={containerRef}
                    className="relative w-full max-w-[800px] h-[600px] mx-auto mt-8 border-3 border-[#ff6b2b] rounded-xl overflow-hidden touch-none bg-[#2a2a2a] shadow-lg"
                    onMouseMove={handleMouseMove}
                    onMouseUp={handleMouseUp}
                    onMouseLeave={handleMouseUp}
                    onTouchMove={handleTouchMove}
                    onTouchEnd={handleTouchEnd}
                    onTouchCancel={handleTouchEnd}
                >
                    <div className="absolute top-4 right-4 px-4 py-2 bg-[#ff6b2b]/90 rounded-full text-white text-sm font-semibold flex items-center gap-2 z-10">
                        🤖 elizaOS Hat
                    </div>

                    {baseImage && (
                        <img src={baseImage} alt="Base" className="w-full h-full object-contain" />
                    )}

                    <div
                        ref={overlayRef}
                        style={getOverlayStyle()}
                        className="absolute top-1/2 left-1/2 cursor-move touch-none filter drop-shadow-lg"
                        onMouseDown={handleMouseDown}
                        onTouchStart={handleTouchStart}
                        onTouchMove={handleTouchMove}
                        onTouchEnd={handleTouchEnd}
                    >
                        <img
                            src={currentHatImage}
                            alt="Overlay"
                            className="w-[100px] h-auto select-none"
                            draggable={false}
                        />
                    </div>
                </div>

                <div className="flex flex-wrap gap-4 justify-center p-6 bg-[#2a2a2a] rounded-xl shadow-lg mt-4 w-full max-w-[800px]">
                    {['⟲ Rotate Left', '⟳ Rotate Right', '+ Scale Up', '- Scale Down', '↔️ Flip', 'Reset', 'Save Image'].map((text) => (
                        <button
                            key={text}
                            onClick={() => {
                                if (text === '⟲ Rotate Left') handleRotate('left');
                                else if (text === '⟳ Rotate Right') handleRotate('right');
                                else if (text === '+ Scale Up') handleScale('up');
                                else if (text === '- Scale Down') handleScale('down');
                                else if (text === '↔️ Flip') handleFlip();
                                else if (text === 'Reset') handleReset();
                                else if (text === 'Save Image') handleSave();
                            }}
                            disabled={text === 'Save Image' && !baseImage}
                            className={`px-6 py-3 rounded-lg bg-[#ff6b2b] text-white cursor-pointer text-base font-semibold uppercase tracking-wider transition-all hover:bg-[#ff8f5a] hover:-translate-y-0.5 hover:shadow-lg active:translate-y-0 disabled:bg-gray-600 disabled:cursor-not-allowed disabled:transform-none`}
                        >
                            {text}
                        </button>
                    ))}
                </div>

                {status && (
                    <div
                        className={`fixed bottom-20 right-8 px-6 py-3 rounded-lg 
                    ${status.type === 'error' ? 'bg-red-500' : 'bg-green-600'}
                    text-white font-medium shadow-lg
                    transition-opacity duration-300
                    opacity-90 animate-fade-out`}
                    >
                        {status.message}
                    </div>
                )}

                <div className="fixed bottom-4 right-4">
                    <a
                        href="https://x.com/ai16zdao"
                        target="_blank"
                        rel="noopener noreferrer"
                        className="inline-flex items-center gap-2 text-sm text-[#ff6b2b] hover:text-[#ff8f5a] transition-colors"
                    >
                        <span>2024 elizaOS</span>
                        <svg
                            className="w-4 h-4 fill-current"
                            xmlns="http://www.w3.org/2000/svg"
                            viewBox="0 0 24 24"
                        >
                            <path d="M18.244 2.25h3.308l-7.227 8.26 8.502 11.24H16.17l-5.214-6.817L4.99 21.75H1.68l7.73-8.835L1.254 2.25H8.08l4.713 6.231zm-1.161 17.52h1.833L7.084 4.126H5.117z" />
                        </svg>
                    </a>
                </div>
            </div>
        </div>
    );
};

default PhotoEditor;

} // namespace elizaos
