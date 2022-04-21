import numpy as np
import matplotlib.image as mpimg
import cv2
import math
#matplotlib inline

def main():
    #cam = cv2.VideoCapture("C:\\Users\\hyung\\Downloads\\Opencv-tester.mp4")
    cam = cv2.VideoCapture("C:\\Users\\hyung\\Downloads\\Opencv-tester4.mp4")

    while(cam.isOpened()):
        ret, frame = cam.read()
        frame_copy = frame.copy()
        if ret:
            height, width = frame_copy.shape[:2]
            cv2.imshow("Video", frame)
            def grayscale(frame) :
                return cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)

            gray = grayscale(frame_copy)
            #cv2.imshow("gray", gray)

            def gaussian_blur(img, kernel_size) :
                return cv2.GaussianBlur(img, (kernel_size, kernel_size), 0)

            kernel_size = 5
            blurredgr = gaussian_blur(gray, kernel_size)
            #cv2.imshow("Blur", blurredgr)

            def canny(img, low_threshold, hight_threshold) :
                return cv2.Canny(img, low_threshold, high_threshold)

            low_threshold = 50
            high_threshold = 200
            edges = canny(blurredgr, low_threshold, high_threshold)
            #cv2.imshow("Canny", edges)

            mask = np.zeros_like(frame_copy)
            #cv2.imshow("Mask", mask)

            if len(frame_copy.shape) > 2 :
                chaneel_count = frame_copy.shape[2]
                ignore_mask_color = (255, )
            else :
                ignore_mask_color = 255
            fmshape = frame_copy.shape
            print(fmshape)

            vertices = np.array([[(525, 410), (625, 325), (700, 325), (745, 410)]], dtype = np.int32)
            #vertices = np.array([[(525, 410), (625, 325), (700, 325), (745, 410)]], dtype = np.int32)
            cv2.fillPoly(mask, vertices, ignore_mask_color)
            cv2.imshow("mask", mask)

            def ROI(img, vertices, color) :
                mask = np.zeros_like(img)
                cv2.fillPoly(mask, vertices, color)
                masked_img = cv2.bitwise_and(img, mask)
                return masked_img

            mask = ROI(edges, vertices, ignore_mask_color)
            cv2.imshow("ROI", mask)
            
            lines = cv2.HoughLinesP(mask, 1, np.pi/180, 30, minLineLength = 0, maxLineGap = 5)

            def draw_lines(img, lines) :
                lines_img = np.zeros_like(img)
                for line in lines :
                    for x1, y1, x2, y2 in line :
                        cv2.line(lines_img, (x1, y1), (x2, y2), (255, 0, 0), 10)
                return lines_img

            def get_fitline(img, lines) :
                lines = np.squeeze(lines)
                lines = lines.reshape(lines.shape[0] * 2, 2)
                output = cv2.fitLine(lines, cv2.DIST_L2, 0, 0.01, 0.01)
                vx, vy, x, y = output[0], output[1], output[2], output[3]
                x1, y1 = int(((img.shape[0] - 1 - y) / vy * vx) + x), img.shape[0] - 1
                x2, y2 = int((((img.shape[0] / 2) + 100 - y) / vy * vx) + x), int((img.shape[0] / 2) + 100)
                result = [x1, y1, x2, y2]
                return result

            def draw_lines2(img, lines, color = [255, 0, 0], thickness = 10) :
                cv2.line(img, (lines[0], lines[1]), (lines[2], lines[3]), color, thickness)

            test_img = draw_lines(mask, lines)
            cv2.imshow("Line1", test_img)

            lines2 = lines.copy()
            lines2 = np.squeeze(lines2)
            #slope_theta = (lines2[:,1] - lines2[:,3]) / (lines2[:,0] - lines2[:,2] + 0.01)
            #lines2 = lines2[np.abs(slope_theta) < 160]
            #slope_theta = slope_theta[np.abs(slope_theta) < 160]

            #fitlines = get_fitline(mask, lines2)
            
            #test2_img = draw_lines2(mask, fitlines)

            #line_img = np.zeros((frame.shape[0], frame.shape[1], 3), dtype = np.uint8)
            #draw_lines(line_img, lines)
            #cv2.imshow("Line2", test2_img)

            def merged_img(frame, initial_img, a = 0.8, b = 1., c = 1.,) :
                return cv2.addWeighted(initial_img, a, frame, b, c)
            
            lines_img = cv2.cvtColor(test_img, cv2.COLOR_GRAY2BGR)
            cv2.imshow("test", lines_img)

            result = merged_img(lines_img, frame_copy)
            cv2.imshow("result", result)

            key = cv2.waitKey(1) & 0xFF
            if (key == 27): #if esc, break
                break    

    cv2.destroyAllWindows()

if __name__ == '__main__' :
    main()