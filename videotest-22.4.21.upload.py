import numpy as np
import matplotlib.image as mpimg
import cv2
import math
#matplotlib inline

def main():
    cam = cv2.VideoCapture("C:\\Users\\hyung\\Downloads\\Opencv-tester4.mp4")

    while(cam.isOpened()):
        ret, frame = cam.read()
        #frame_copy = frame.copy()
        if ret:
            frame_copy = frame.copy()
            height, width = frame_copy.shape[:2]
            cv2.imshow("Video", frame)
            def grayscale(frame) :
                return cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY) #convert to gray

            gray = grayscale(frame_copy)

            def gaussian_blur(img, kernel_size) :
                return cv2.GaussianBlur(img, (kernel_size, kernel_size), 0) # img to bluured

            kernel_size = 5
            blurredgr = gaussian_blur(gray, kernel_size)

            def canny(img, low_threshold, hight_threshold) :
                return cv2.Canny(img, low_threshold, high_threshold) # canny edge exec

            low_threshold = 50
            high_threshold = 200
            edges = canny(blurredgr, low_threshold, high_threshold)

            mask = np.zeros_like(frame_copy)

            if len(frame_copy.shape) > 2 : 
                chaneel_count = frame_copy.shape[2] 
                ignore_mask_color = (255, )# if not null blue
            else :
                ignore_mask_color = 255
            fmshape = frame_copy.shape # if null black
            print(fmshape)

            vertices = np.array([[(525, 410), (625, 325), (700, 325), (745, 410)]], dtype = np.int32) # set range
            cv2.fillPoly(mask, vertices, ignore_mask_color) # fill blue -> for checking estimated roi 

            def ROI(img, vertices, color) :
                mask = np.zeros_like(img)
                cv2.fillPoly(mask, vertices, color)
                masked_img = cv2.bitwise_and(img, mask) 
                return masked_img

            mask = ROI(edges, vertices, ignore_mask_color)# set roi
            
            lines = cv2.HoughLinesP(mask, 1, np.pi/180, 30, minLineLength = 0, maxLineGap = 5) # inspect houghs

            def draw_lines(img, lines) :
                lines_img = np.zeros_like(img)
                for line in lines :
                    for x1, y1, x2, y2 in line :
                        cv2.line(lines_img, (x1, y1), (x2, y2), (255, 0, 0), 10)
                return lines_img

            test_img = draw_lines(mask, lines)

            def merged_img(frame, initial_img, a = 0.8, b = 1., c = 1.,) : # merge initial_img & frame
                return cv2.addWeighted(initial_img, a, frame, b, c)
            
            lines_img = cv2.cvtColor(test_img, cv2.COLOR_GRAY2BGR)

            result = merged_img(lines_img, frame_copy)
            cv2.imshow("result", result)

            key = cv2.waitKey(1) & 0xFF
            if (key == 27): #if esc, break
                break    
        else :
            break

    cv2.destroyAllWindows()

if __name__ == '__main__' :
    main()